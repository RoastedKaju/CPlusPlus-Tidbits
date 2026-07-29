#include <iostream>
#include <utility>
#include <vector>
#include <cassert>
#include <atomic>
#include <bitset>
#include <cstdint>
#include <limits>
#include <memory>

// constants
constexpr uint32_t kMaxComponentTypes = 32;
constexpr uint32_t kMaxEntityCount = 10'000;
constexpr uint32_t kInvalidIndex = std::numeric_limits<uint32_t>::max();

// alias
using EntityID = uint64_t;
using ComponentTypeID = uint32_t;
using EntityIndex = uint32_t;
using EntityGeneration = uint32_t;
using ComponentMask = std::bitset<kMaxComponentTypes>;

// non-RTTI component ID generator
static std::atomic<ComponentTypeID> nextComponentID = 0;

template<typename T>
ComponentTypeID get_component_id() {
    static ComponentTypeID id = nextComponentID.fetch_add(1, std::memory_order_relaxed);
    assert(id < kMaxComponentTypes && "Exceeded max component type limit.");
    return id;
}

// helper functions
EntityID create_entity(const EntityIndex index, const EntityGeneration gen) {
    return static_cast<EntityID>(index) << 32 | (gen);
}

EntityIndex get_index(const EntityID id) {
    return id >> 32;
}

EntityGeneration get_generation(const EntityID id) {
    return static_cast<EntityGeneration>(id);
}

// storage
struct Storage {
    virtual ~Storage() = default;

    [[nodiscard]] virtual void *get(std::size_t) const = 0;

    virtual void destroy(std::size_t) = 0;
};

/**
 * Pool storage
 */
template<typename T>
struct PoolStorage final : public Storage {
    PoolStorage() {
        element_size_ = sizeof(T);
        alignment = alignof(T);

        buffer_ = ::operator new(element_size_ * kMaxEntityCount, static_cast<std::align_val_t>(alignment));
    }

    ~PoolStorage() override {
        ::operator delete(buffer_, static_cast<std::align_val_t>(alignment));
    }

    [[nodiscard]] void *get(const std::size_t index) const override {
        return static_cast<std::byte *>(buffer_) + index * element_size_;
    }

    void destroy(const std::size_t index) override {
        T *comp_ptr = static_cast<T *>(get(index));
        comp_ptr->~T();
    }

private:
    void *buffer_{nullptr};
    std::size_t element_size_{0};
    std::size_t alignment{0};
};

/**
 * Entity manager
 */
struct Manager {
    struct EntityDesc {
        EntityGeneration gen{0};
        ComponentMask mask{};
        bool alive{false};
    };

    [[nodiscard]] bool is_alive(const EntityID id) const {
        const EntityIndex index = get_index(id);

        if (index > entities_.size()) {
            return false;
        }

        const auto &entity_desc = entities_[index];
        return entity_desc.alive && entity_desc.gen == get_generation(id);
    }

    EntityID create() {
        EntityIndex index;

        // check if we have a free slot
        if (!free_indices_.empty()) {
            index = free_indices_.back();
            free_indices_.pop_back();
        } else {
            assert(entities_.size() < kMaxEntityCount && "Entity limit reached.");

            index = entities_.size();
            entities_.emplace_back(EntityDesc{});
        }

        auto &entity_desc = entities_[index];
        entity_desc.alive = true;

        return create_entity(index, entity_desc.gen);
    }

    void destroy(const EntityID id) {
        if (!is_alive(id)) {
            return;
        }

        const EntityIndex index = get_index(id);
        auto &[gen, mask, alive] = entities_[index];

        // clean up all the components attached to this entity
        for (auto i = 0; i < pools_.size(); ++i) {
            // only destroy components that pass the bit test
            if (mask.test(i)) {
                pools_[i]->destroy(index);
            }
        }
        // zero out all the bits
        mask.reset();
        // bump up the generation counter
        alive = false;
        ++gen;

        // add to free list
        free_indices_.push_back(index);
    }

    template<typename T>
    [[nodiscard]] bool has_component(const EntityID id) const {
        if (!is_alive(id)) {
            return false;
        }

        const EntityIndex index = get_index(id);
        const ComponentTypeID type_id = get_component_id<T>();

        return entities_[index].mask.test(type_id);
    }

    template<typename T, typename... Args>
    T *add_component(const EntityID id, Args &&... args) {
        // check if alive
        assert(is_alive(id) && "Entity is not alive.");

        const EntityIndex index = get_index(id);
        const ComponentTypeID type_id = get_component_id<T>();

        // check if entity already has component
        assert(!entities_[index].mask.test(type_id) && "Entity already has this component.");

        // if there is no pool for this type of component, reserve
        if (pools_.size() <= type_id) {
            pools_.resize(type_id + 1);
        }
        // if pool is not initialized, create it
        if (!pools_[type_id]) {
            pools_[type_id] = std::make_unique<PoolStorage<T> >();
        }

        // placement new the component
        void *memory_loc = pools_[type_id]->get(index);
        T *comp = new(memory_loc) T(std::forward<Args>(args)...);

        // mark the component bit as set
        entities_[index].mask.set(type_id, true);

        return comp;
    }

    template<typename T>
    void remove_component(const EntityID id) {
        if (!is_alive(id)) {
            return;
        }

        const EntityIndex index = get_index(id);
        const ComponentTypeID type_id = get_component_id<T>();

        // if entity has this component
        if (!entities_[index].mask.test(type_id)) {
            return;
        }

        // destroy component
        pools_[type_id]->destroy(index);

        // reset bit mask
        entities_[index].mask.reset(type_id);
    }

    template<typename T>
    T *get_component(const EntityID id) {
        if (!is_alive(id)) {
            return nullptr;
        }

        const EntityIndex index = get_index(id);
        const ComponentTypeID type_id = get_component_id<T>();

        if (!entities_[index].mask.test(type_id)) {
            return nullptr;
        }

        // get component
        T *comp_ptr = pools_[type_id]->get(index);

        return comp_ptr;
    }

    template<typename T>
    T *get_component_internal(const EntityIndex index) {
        const ComponentTypeID type_id = get_component_id<T>();
        if (type_id >= pools_.size() || !pools_[type_id]) {
            return nullptr;
        }

        return static_cast<T *>(pools_[type_id]->get(index));
    }

    std::vector<EntityDesc> &get_entities() {
        return entities_;
    }

private:
    std::vector<EntityDesc> entities_;
    std::vector<EntityIndex> free_indices_;
    std::vector<std::unique_ptr<Storage> > pools_;
};

template<typename... Types>
struct View {
    explicit View(Manager &manager) : manager{manager} {
        (required_mask.set(get_component_id<Types>()), ...);
    }

    template<typename F>
    void each(F &&func) {
        for (EntityIndex i = 0; i < manager.get_entities().size(); ++i) {
            const auto &[gen, mask, alive] = manager.get_entities()[i];
            if (!alive) {
                continue;
            }
            if ((mask & required_mask) != required_mask) {
                continue;
            }

            EntityID id = create_entity(i, gen);
            func(id, *manager.get_component_internal<Types>(i)...);
        }
    }

private:
    Manager &manager;
    ComponentMask required_mask{};
};

// dummy components
struct TransformComponent {
    TransformComponent() = default;

    explicit TransformComponent(std::string name) : debugName{std::move(name)} {
    }

    std::string debugName{};
};

struct SpriteComponent {
    // ...
};

struct MovementComponent {
    // ...
};


int main() {
    Manager manager{};
    const auto e1 = manager.create();
    manager.add_component<TransformComponent>(e1, "Player");
    manager.add_component<SpriteComponent>(e1);

    const auto e2 = manager.create();
    manager.add_component<TransformComponent>(e2, "Enemy");
    manager.add_component<MovementComponent>(e2);

    View<TransformComponent, SpriteComponent> view{manager};
    view.each([](const EntityID id, const TransformComponent &transform, SpriteComponent &sprite) {
        std::cout << "Entity: " << get_index(id) << " name: " << transform.debugName << "\n";
    });

    return EXIT_SUCCESS;
}

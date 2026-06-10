#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <atomic>
#include <bitset>

namespace ecs
{
    struct TransformComponent
    {
        TransformComponent() = default;
        TransformComponent(const std::string &name) : debugName{name} {}

        std::string debugName{};
    };
    struct SpriteComponent
    {
        // ...
    };
    struct MovementComponent
    {
        // ...
    };

    constexpr uint32_t MAX_ENTITY_COUNT = 10'000;
    constexpr uint32_t MAX_COMPONENT_TYPES = 32;
    constexpr uint32_t INVALID_ENTITY_INDEX = std::numeric_limits<uint32_t>::max();

    using EntityID = uint64_t;
    using ComponentTypeID = uint32_t;
    using EntityIndex = uint32_t;
    using EntityGeneration = uint32_t;
    using ComponentMask = std::bitset<MAX_COMPONENT_TYPES>;

    static std::atomic<ComponentTypeID> nextComponentID = 0;
    // Get component type ID
    template <typename T>
    ComponentTypeID GetComponentTypeID()
    {
        static ComponentTypeID ID = nextComponentID.fetch_add(1, std::memory_order_relaxed);
        assert(ID < MAX_COMPONENT_TYPES && "Exceeded max component types");
        return ID;
    }

    // Create entity ID by combinding index and version
    // First 32 bits are index, last 32 bits are version
    inline EntityID CreateEntityID(EntityIndex index, EntityGeneration generation)
    {
        return ((EntityID)index << 32) | ((EntityID)generation);
    }
    inline EntityIndex GetEntityIndex(EntityID entity)
    {
        return entity >> 32;
    }
    inline EntityGeneration GetEntityGeneration(EntityID entity)
    {
        return static_cast<EntityGeneration>(entity);
    }

    struct IStorage
    {
        virtual ~IStorage() = default;
        virtual void *Get(size_t index) = 0;
        virtual void Destroy(size_t index) = 0;
    };

    // Fixed size pool
    // Pool storage allocates memory for all the possible components
    // Data is aligned
    // void pointer to memory
    template <typename T>
    struct PoolStorage : public IStorage
    {
        void *data{nullptr};
        size_t elementSize{0};
        size_t alignment{0};

        PoolStorage()
        {
            elementSize = sizeof(T);
            alignment = alignof(T);

            data = ::operator new(elementSize * MAX_ENTITY_COUNT, std::align_val_t(alignment));
        }

        ~PoolStorage()
        {
            ::operator delete(data, std::align_val_t(alignment));
        }

        void *Get(size_t index) override
        {
            return static_cast<char *>(data) + index * elementSize;
        }

        void Destroy(size_t index) override
        {
            // gets the pointer to this component's memory location
            T *component = static_cast<T *>(Get(index));
            component->~T();
        }
    };

    // Acts as an entity manager
    // keeps track of all entities in a list
    // has a free list for reuse
    // handles storage
    struct Scene
    {
        // All the information about entity
        // minus the entity index as the entity index will map 1-1 directly with our entities vector
        struct EntityDesc
        {
            EntityGeneration generation{0};
            ComponentMask mask{};
            bool alive{false};
        };

        // tracking lists
        std::vector<EntityDesc> entities;
        std::vector<EntityIndex> freeIndices; // use stack its better for LIFO
        std::vector<std::unique_ptr<IStorage>> componentPools;
        // can also add list of alive indices, but you will have to do extra book keeping in create and destroy

        bool IsAlive(EntityID id) const
        {
            EntityIndex index = GetEntityIndex(id);

            if (index >= entities.size())
            {
                return false;
            }

            const auto &entityDesc = entities[index];

            return entityDesc.alive && entityDesc.generation == GetEntityGeneration(id);
        }

        EntityID CreateEntity()
        {
            EntityIndex index;

            // check if we can reuse some slot
            if (!freeIndices.empty())
            {
                index = freeIndices.back();
                freeIndices.pop_back();
            }
            else
            {
                assert(entities.size() < MAX_ENTITY_COUNT && "Cannot create more entities than max entity count.");

                index = static_cast<EntityIndex>(entities.size());
                entities.emplace_back(EntityDesc{});
            }

            auto &entityDesc = entities[index];
            entityDesc.alive = true;

            return CreateEntityID(index, entityDesc.generation);
        }

        void DestroyEntity(EntityID id)
        {
            if (!IsAlive(id))
            {
                return;
            }

            EntityIndex index = GetEntityIndex(id);
            auto &entityDesc = entities[index];

            // clean up all the components attached to this entity
            for (size_t i = 0; i < componentPools.size(); ++i)
            {
                if (entityDesc.mask.test(i))
                {
                    componentPools[i]->Destroy(index);
                }
            }

            // reset component mask
            entityDesc.mask.reset();

            // bump up the generation counter
            entityDesc.alive = false;
            ++entityDesc.generation;

            // add to free list
            freeIndices.push_back(index);
        }

        // Has component
        template <typename T>
        bool HasComponent(EntityID id)
        {
            // check if entity is alive
            if (!IsAlive(id))
            {
                return false;
            }

            EntityIndex index = GetEntityIndex(id);
            ComponentTypeID componentID = GetComponentTypeID<T>();

            return entities[index].mask.test(componentID);
        }

        // Add component to entity
        template <typename T, typename... Args>
        T *AddComponent(EntityID id, Args &&...args)
        {
            // check if entity is alive
            assert(IsAlive(id) && "To add a component entity should be alive");

            EntityIndex index = GetEntityIndex(id);
            ComponentTypeID componentID = GetComponentTypeID<T>();

            // check if entity already has this component
            assert(!entities[index].mask.test(componentID) && "Entity already has this component");

            // If this type already doesn't have a pool then reserve
            if (componentPools.size() <= componentID)
            {
                // since component type ids start from 0, add 1 to get the new size
                componentPools.resize(componentID + 1);
            }

            // if pool is not initialized then create it
            if (!componentPools[componentID])
            {
                componentPools[componentID] = std::make_unique<PoolStorage<T>>();
            }

            // get memory location
            void *componentMemory = componentPools[componentID]->Get(index);
            // construct component in pool (inplace)
            T *component = new (componentMemory) T(std::forward<Args>(args)...);

            // mark the bit set that we have added this component
            entities[index].mask.set(componentID);

            return component;
        }

        // Remove component from entity
        template <typename T>
        void RemoveComponent(EntityID id)
        {
            // check if entity is alive
            if (!IsAlive(id))
            {
                return;
            }

            EntityIndex index = GetEntityIndex(id);
            ComponentTypeID componentID = GetComponentTypeID<T>();

            // check if entity has this component
            if (!entities[index].mask.test(componentID))
            {
                return;
            }

            // destroy component
            componentPools[componentID]->Destroy(index);

            // reset bit mask
            entities[index].mask.reset(componentID);
        }

        // Get Component
        template <typename T>
        T *GetComponent(EntityID id)
        {
            // Check if Entity is alive
            if (!IsAlive(id))
            {
                return nullptr;
            }

            EntityIndex index = GetEntityIndex(id);
            ComponentTypeID componentID = GetComponentTypeID<T>();

            // Check if entity has component
            if (!entities[index].mask.test(componentID))
            {
                return nullptr;
            }

            // get component
            T *component = static_cast<T *>(componentPools[componentID]->Get(index));

            return component;
        }

        // Get Component Internal
        template <typename T>
        T *GetComponentInternal(EntityIndex index)
        {
            ComponentTypeID componentID = GetComponentTypeID<T>();
            if (componentID >= componentPools.size() || !componentPools[componentID])
            {
                return nullptr;
            }

            return static_cast<T *>(componentPools[componentID]->Get(index));
        }
    };

    template <typename... Types>
    struct View
    {
        Scene &scene;
        ComponentMask requiredMask;

        View(Scene &scene) : scene{scene}
        {
            (requiredMask.set(GetComponentTypeID<Types>()), ...);
        }

        template <typename Func>
        void Each(Func &&func)
        {
            for (EntityIndex i = 0; i < scene.entities.size(); ++i)
            {
                const auto &desc = scene.entities[i];
                if (!desc.alive)
                {
                    continue;
                }
                if ((desc.mask & requiredMask) != requiredMask)
                {
                    continue;
                }

                EntityID id = CreateEntityID(i, desc.generation);
                func(id, *scene.GetComponentInternal<Types>(i)...);
            }
        }
    };

    void driver()
    {
        Scene scene{};

        auto e1 = scene.CreateEntity();
        scene.AddComponent<TransformComponent>(e1, "Player");
        scene.AddComponent<MovementComponent>(e1);

        auto e2 = scene.CreateEntity();
        scene.AddComponent<TransformComponent>(e2, "Enemy");
        scene.AddComponent<MovementComponent>(e2);

        // Only E1 matches both
        View<TransformComponent, MovementComponent> view{scene};
        view.Each([](EntityID id, TransformComponent &transformComp, MovementComponent &movementComp)
                  { std::cout << "Entity: " << GetEntityIndex(id) << " name: " << transformComp.debugName << "\n"; });
    }
}
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
        // ...
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

    template <typename T>
    struct PoolStorage : public IStorage
    {
        // Fixed size pool
        // Pool storage allocates memory for all the possible components
        // Data is aligned
        // void pointer to memory

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
        std::vector<EntityIndex> freeIndices;
        std::vector<std::unique_ptr<IStorage>> componentPools;

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

        // Add component to entity

        // Remove component from entity
    };

    void driver()
    {
        Scene scene{};

        scene.CreateEntity();
        scene.CreateEntity();
        scene.CreateEntity();

        // scene.DestroyEntity(1);

        std::cout << scene.freeIndices.size() << std::endl;
    }
}
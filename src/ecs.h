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
    };

    // Acts as an entity manager
    // keeps track of all entities in a list
    // has a free list for reuse
    // handles storage
    struct Scene
    {

    };

    void driver()
    {
        EntityID entity = CreateEntityID(0, 1);
        EntityID anotherEntity = CreateEntityID(2, 5);

        PoolStorage<TransformComponent> transformPool{};

        // get the memory pointer at index
        void *memory = transformPool.Get(GetEntityIndex(entity));
        // construct in-place the component
        new (memory) TransformComponent();

        // Check if the component has been created in pool
        auto *comp = static_cast<TransformComponent *>(memory);
        comp->debugName = "Transform";

        std::cout << comp->debugName << std::endl;
    }
}
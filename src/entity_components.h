#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>

namespace ecs
{
    static constexpr size_t MAX_ENTITIES = 10'000;
    static constexpr uint32_t INVALID_INDEX = 0xFFFFFFFF;

    // <Entity And Components>
    struct Entity
    {
        Entity(uint32_t _id, uint32_t _generation) : id{_id}, generation{_generation}
        {
            std::cout << "Entity created with ID: " << id << '\n';
        }

        uint32_t id;
        uint32_t generation;
    };

    // <Managers>
    struct EntityManager
    {
        std::vector<uint32_t> free_ids;
        std::vector<uint32_t> generations; // index = ID, value = current generation
        uint32_t next_id{0};

        Entity create_entity()
        {
            uint32_t new_id;
            uint32_t current_gen;

            if (!free_ids.empty())
            {
                new_id = free_ids.back();
                free_ids.pop_back();
                current_gen = generations[new_id];
            }
            else
            {
                if (next_id >= MAX_ENTITIES)
                {
                    throw std::runtime_error("Entity Limit Reached.");
                }

                new_id = next_id++;
                generations.push_back(0); // start new ID at generation 0
                current_gen = 0;
            }

            return Entity{new_id, current_gen};
        }

        void destroy_entity(Entity entity)
        {
            if (is_alive(entity))
            {
                // invalidate old handles by incrementing generation count
                generations[entity.id]++;
                free_ids.push_back(entity.id);
                std::cout << "Entity destroyed with ID: " << entity.id << '\n';
            }
        }

        bool is_alive(Entity entity) const
        {
            // make sure we are in-bounds
            if (entity.id >= generations.size())
            {
                return false;
            }

            // Entity is only alive if the generation matches registry's generation
            return entity.generation == generations[entity.id];
        }
    };

    // <Sparse set container struct>
    template <typename T>
    struct ComponentSet
    {
        std::vector<T> dense_components;       // tightly packed components
        std::vector<uint32_t> dense_to_entity; // dense index mapped to entity ID, needed for swap and pop
        std::vector<uint32_t> entity_to_dense; // entity ID mapped to component, initialize with invalid index

        ComponentSet()
        {
            entity_to_dense.resize(MAX_ENTITIES, INVALID_INDEX);
        }

        void insert(uint32_t id, const T &component)
        {
            if (id >= MAX_ENTITIES)
            {
                throw std::runtime_error("Entity ID out of bounds.");
            }
            if (entity_to_dense[id] != INVALID_INDEX)
            {
                throw std::runtime_error("Component already exists for this entity");
            }

            // Add new component at the end of dense list
            size_t new_index = dense_components.size();
            entity_to_dense[id] = static_cast<uint32_t>(new_index);

            dense_components.push_back(component);
            dense_to_entity.push_back(id);
        }

        void remove(uint32_t id)
        {
            if (id >= MAX_ENTITIES || entity_to_dense[id] == INVALID_INDEX)
            {
                return;
            }

            uint32_t index_of_removed = entity_to_dense[id];
            uint32_t index_of_last = static_cast<uint32_t>(dense_components.size() - 1);

            // Swap and Pop
            if (index_of_removed != index_of_last)
            {
                dense_components[index_of_removed] = std::move(dense_components[index_of_last]);

                uint32_t entity_of_last = dense_to_entity[index_of_last];
                dense_to_entity[index_of_removed] = entity_of_last;

                // Update sparse array
                entity_to_dense[entity_of_last] = index_of_removed;
            }

            // Invalidate component
            entity_to_dense[id] = INVALID_INDEX;

            // Shrink arrays
            dense_components.pop_back();
            dense_to_entity.pop_back();
        }

        T &get(uint32_t id)
        {
            if (id >= MAX_ENTITIES || entity_to_dense[id] == INVALID_INDEX)
            {
                throw std::runtime_error("Component requested does not exist for this entity.");
            }

            return dense_components[entity_to_dense[id]];
        }
    };

    // Interface so we can store different components in a single container
    struct IComponentSet
    {
        virtual ~IComponentSet() = default;
        virtual void entity_destroyed(uint32_t id) = 0;
    };

    template <typename T>
    struct ComponentSetInstance : public ComponentSet<T>, public IComponentSet
    {
        void entity_destroyed(uint32_t id) override
        {
            this->remove(id);
        }
    };

    // <Unique compile time IDs for Component types>
    static uint32_t component_type_id = 0;

    template <typename T>
    struct ComponentTypeID
    {
        static uint32_t value()
        {
            static uint32_t id = component_type_id++;
            return id;
        }
    };

    struct PositionComponent
    {
        float x, y;
    };

    struct ComponentManager
    {
        // Component type ID -> component set map
        std::unordered_map<uint32_t, std::shared_ptr<IComponentSet>> components_map;

        template <typename T>
        ComponentSetInstance<T> &get_or_create_set()
        {
            uint32_t id = ComponentTypeID<T>::value();
            auto [it, inserted] = components_map.try_emplace(id, std::make_shared<ComponentSetInstance<T>>());
            return *static_cast<ComponentSetInstance<T> *>(it->second.get());
        }

        template <typename T>
        void add_component(Entity entity, const T &component)
        {
            auto &set = get_or_create_set<T>();
            set.insert(entity.id, component);
        }

        template <typename T>
        void remove_component(Entity entity)
        {
            uint32_t id = ComponentTypeID<T>::value();
            auto it = components_map.find(id);
            if (it == components_map.end())
            {
                throw std::runtime_error("Attemped to remove component type that isn't registered");
            }

            it->second->entity_destroyed(entity.id);
        }
    };

    struct Registry
    {
    };

    // <Main Function>
    void driver_function()
    {
        try
        {
            EntityManager entity_manager{};
            ComponentManager component_manager{};

            std::cout << "--- Creating Entities ---\n";
            Entity e1 = entity_manager.create_entity(); // ID: 0, Gen: 0
            Entity e2 = entity_manager.create_entity(); // ID: 1, Gen: 0

            std::cout << "\n--- Adding Components ---\n";
            component_manager.add_component(e1, PositionComponent{10.0f, 20.0f});
            component_manager.add_component(e2, PositionComponent{30.0f, 40.0f});

            // Retrieve and print
            auto &posSet = component_manager.get_or_create_set<PositionComponent>();
            std::cout << "e1 Position: (" << posSet.get(e1.id).x << ", " << posSet.get(e1.id).y << ")\n";
            std::cout << "e2 Position: (" << posSet.get(e2.id).x << ", " << posSet.get(e2.id).y << ")\n";

            std::cout << "\n--- Removing Component from e1 ---\n";
            component_manager.remove_component<PositionComponent>(e1);

            // Try to access removed component (should throw)
            try
            {
                posSet.get(e1.id);
            }
            catch (const std::exception &ex)
            {
                std::cout << "Accessing removed component failed as expected: " << ex.what() << '\n';
            }

            std::cout << "\n--- Destroying e2 ---\n";
            entity_manager.destroy_entity(e2);
            // You could also notify ComponentManager here to clean up all components for e2
        }
        catch (const std::exception &exception)
        {
            std::cerr << exception.what() << '\n';
        }
    }
}
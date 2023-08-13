#pragma once

#include <concepts>
#include <vector>

template <typename T>
concept ComponentType = std::derived_from<T, corn::Component>;


namespace corn {
    class Entity {
    private:
        unsigned int uniqueId;
        std::vector<Component*> components;

    public:
        Entity();
        ~Entity();

        /**
         * Attaching a component to the entity if not already exist
         * @tparam T Type of the component
         * @return Pointer to the component if successfully added, else null
         */
        template <ComponentType T>
        T* addComponent();

        /**
         * Obtain the corresponding component if exist
         * @tparam T Type of the component
         * @return Pointer to the component if exist, else null
         */
        template <ComponentType T>
        T* getComponent();

        /**
         * Removing a component from the entity if exist
         * @tparam T Type of the component
         * @return Whether the component is successfully removed
         */
        template <ComponentType T>
        bool removeComponent();
    };


    class EntityManager {
    private:
        std::vector<Entity> entities;

    public:
        EntityManager();
        ~EntityManager() = default;
        // TODO: add/get/remove entities
    };


    class ECamera : public Entity {};
}

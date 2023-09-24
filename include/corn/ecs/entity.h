#pragma once

#include <concepts>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <corn/ecs/component.h>


namespace corn {
    template <typename T>
    concept ComponentType = std::derived_from<T, corn::Component>;

    class EntityManager;

    /**
     * @class Entity
     * @brief Entity in the ECS architecture.
     *
     * The Entity class does nothing but serves as a container for Components. They should not be created or destroyed
     * directly. Instead, their lifetime is managed by the Entity Manager.
     *
     * @see EntityManager
     * @see Component
     * @see System
     */
    class Entity final {
    public:
        using EntityID = unsigned long long int;
        // EntityManager need access to ctor/dtor
        friend class EntityManager;

        /// @brief Indicates whether the Entity is active.
        bool active;
        /// @brief Reference to the Entity Manager that created this Entity.
        EntityManager& entityManager;

        /**
         * @brief The unique ID of the Entity.
         *
         * The unique ID starts from 0 and adds one for each new Entity created within the same scene. After it reaches
         * the max value of EntityID, it goes back to 0.
         */
        const EntityID id;
        /**
         * @brief The name of the Entity.
         *
         * Unlike ID, the name is a mutable property assigned during creation. Multiple Entities are allowed to have
         * the same name.
         */
        std::string name;

        /// @brief Destroys the Entity itself.
        void destroy();

        /**
         * @brief Attach a Component to the Entity if not already exists.
         * @tparam T Type of the Component, must derive from Component class.
         * @param args Arguments for constructing the Component (excluding the first argument Entity& entity)
         * @return Pointer to the Component if successfully added, else null pointer.
         */
        template <ComponentType T, typename... Args>
        T* addComponent(Args&&... args);

        /**
         * @brief Obtain the corresponding Component if exists.
         * @tparam T Type of the Component, must derive from Component class.
         * @return Pointer to the Component if exists, else null pointer.
         */
        template <ComponentType T>
        T* getComponent();

        /**
         * @brief Removing a Component from the Entity if exists.
         * @tparam T Type of the Component, must derive from Component class.
         * @return Whether the Component originally exists.
         */
        template <ComponentType T>
        bool removeComponent();

        /**
         * Get the parent Entity
         * @return
         */
        Entity* getParent() const;

        std::vector<Entity*> getChildren() const;

    private:
        /// List of all attached Components
        std::unordered_map<std::type_index, Component*> components;

        // Private constructor/destructor
        explicit Entity(EntityID id, std::string name, EntityManager& entityManager);
        ~Entity();
        Entity(const Entity& other) = delete;
    };
}

#include <corn/ecs/entity_template.cpp>

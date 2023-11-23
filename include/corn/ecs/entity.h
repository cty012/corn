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
    concept ComponentType = std::derived_from<T, Component>;

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
        using EntityID = size_t;
        // EntityManager need access to ctor/dtor
        friend class EntityManager;

        /// @brief Indicates whether the Entity is active.
        bool active;

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
         * @return Whether the Entity is active. An Entity is active iff itself and all its ancestors have
         * Entity::active set to true.
         */
        [[nodiscard]] bool isActive() const;

        /// @return The Entity manager that owns this Entity.
        [[nodiscard]] EntityManager& getEntityManager() const;

        /// @return The scene that contains this Entity.
        [[nodiscard]] Scene& getScene() const;

        /// @return The game that contains this Entity.
        [[nodiscard]] const Game* getGame() const;

        /**
         * @brief Create a Component and attach it to the Entity.
         * @tparam T Type of the Component, must derive from Component class.
         * @param args Arguments for constructing the Component (excluding the first argument Entity& entity).
         * @return Pointer to the Component if successfully added, else null pointer.
         *
         * If a component of the same type already exist, it will NOT be replaced.
         */
        template <ComponentType T, typename... Args>
        T* createComponent(Args&&... args);

        /**
         * @brief Obtain the corresponding Component.
         * @tparam T Type of the Component, must derive from Component class.
         * @return Pointer to the Component if exists, else null pointer.
         */
        template <ComponentType T>
        T* getComponent() const;

        /**
         * @brief Removing a Component from the Entity.
         * @tparam T Type of the Component, must derive from Component class.
         * @return Whether the Component originally exists.
         */
        template <ComponentType T>
        bool removeComponent();

        /// @return Get the parent Entity.
        Entity* getParent() const;

        /// @return Get the list of child Entities.
        std::vector<Entity*> getChildren() const;

    private:
        /// @brief The Entity manager that owns this Entity.
        EntityManager& entityManager;

        /// List of all attached Components
        std::unordered_map<std::type_index, Component*> components;

        // Private constructor/destructor
        explicit Entity(EntityID id, std::string name, EntityManager& entityManager);
        ~Entity();
        Entity(const Entity& other) = delete;
        Entity& operator=(const Entity& other) = delete;
    };

    template<ComponentType T, typename... Args>
    T* Entity::createComponent(Args&&... args) {
        auto key = std::type_index(typeid(T));
        if (this->components.find(key) != this->components.end()) return nullptr;
        T* component = new T(*this, std::forward<Args>(args)...);
        this->components[key] = component;
        return component;
    }

    template<ComponentType T>
    T* Entity::getComponent() const {
        auto key = std::type_index(typeid(T));
        if (this->components.find(key) == this->components.end()) return nullptr;
        return dynamic_cast<T*>(this->components.at(key));
    }

    template<ComponentType T>
    bool Entity::removeComponent() {
        auto key = std::type_index(typeid(T));
        if (this->components.find(key) == this->components.end()) return false;
        this->components.erase(key);
        return true;
    }
}

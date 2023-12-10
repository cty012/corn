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
     * The Entity class does nothing but serves as a container for components. They should not be created or destroyed
     * directly. Instead, their lifetime is managed by the entity manager.
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

        /// @brief Getter for the entity's ID.
        [[nodiscard]] EntityID getID() const noexcept;

        /// @brief Getter for the entity's name.
        [[nodiscard]] const std::string& getName() const noexcept;

        /// @brief Setter for the entity's name.
        void setName(std::string name) noexcept;

        /// @return Getter for the entity's active property.
        [[nodiscard]] bool isActive() const noexcept;

        /// @brief Setter for the entity's active property.
        void setActive(bool active) noexcept;

        /**
         * @return Whether the entity is active in the world.
         *
         * An entity is active in the world if and only if itself and all its ancestors have property active set to
         * true.
         */
        [[nodiscard]] bool isActiveInWorld() const noexcept;

        /// @return The entity manager that owns this entity.
        [[nodiscard]] EntityManager& getEntityManager() const noexcept;

        /// @return The scene that contains this entity.
        [[nodiscard]] Scene& getScene() const noexcept;

        /// @return The game that contains this entity.
        [[nodiscard]] const Game* getGame() const noexcept;

        /// @brief Destroys the entity itself.
        void destroy() noexcept;

        /**
         * @brief Create a component and attach it to the entity.
         * @tparam T Type of the component, must derive from Component class.
         * @param args Arguments for constructing the component (excluding the first argument Entity& entity).
         * @return Pointer to the component if successfully added, else null pointer.
         *
         * If a component of the same type already exist, it will NOT be replaced. Instead null pointer will be
         * returned.
         */
        template <ComponentType T, typename... Args>
        T* addComponent(Args&&... args);

        /**
         * @brief Obtain the corresponding component.
         * @tparam T Type of the component, must derive from Component class.
         * @return Pointer to the component if exists, else null pointer.
         */
        template <ComponentType T>
        T* getComponent() const noexcept;

        /**
         * @brief Removing a component from the entity.
         * @tparam T Type of the component, must derive from Component class.
         * @return Whether the component originally exists.
         */
        template <ComponentType T>
        bool removeComponent() noexcept;

        /// @return Get the parent entity.
        [[nodiscard]] Entity* getParent() const noexcept;

        /// @return Get the list of child entities.
        [[nodiscard]] std::vector<Entity*> getChildren() const noexcept;

    private:
        /// @brief Constructor.
        explicit Entity(EntityID id, std::string name, EntityManager& entityManager) noexcept;

        /// @brief Destructor.
        ~Entity();

        Entity(const Entity& other) = delete;
        Entity& operator=(const Entity& other) = delete;

        /**
         * @brief The unique ID of the entity.
         *
         * The unique ID starts from 0 and adds one for each new entity created within the same scene. After it reaches
         * the max value of EntityID, it goes back to 0.
         */
        const EntityID id_;

        /**
         * @brief The name of the entity.
         *
         * Unlike ID, the name is a mutable property assigned during creation. Multiple Entities are allowed to have
         * the same name.
         */
        std::string name_;

        /// @brief Indicates whether the entity is active.
        bool active_;

        /// @brief The entity manager that owns this entity.
        EntityManager& entityManager_;

        /// @brief List of all attached components.
        std::unordered_map<std::type_index, Component*> components_;
    };

    template<ComponentType T, typename... Args>
    T* Entity::addComponent(Args&&... args) {
        auto key = std::type_index(typeid(T));
        if (this->components_.find(key) != this->components_.end()) return nullptr;
        T* component = new T(*this, std::forward<Args>(args)...);
        this->components_[key] = component;
        return component;
    }

    template<ComponentType T>
    T* Entity::getComponent() const noexcept {
        auto key = std::type_index(typeid(T));
        if (this->components_.find(key) == this->components_.end()) return nullptr;
        return dynamic_cast<T*>(this->components_.at(key));
    }

    template<ComponentType T>
    bool Entity::removeComponent() noexcept {
        auto key = std::type_index(typeid(T));
        if (this->components_.find(key) == this->components_.end()) return false;
        this->components_.erase(key);
        return true;
    }
}

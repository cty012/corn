#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <corn/ecs/entity.h>
#include <corn/event/event_args.h>
#include <corn/event/event_manager.h>

namespace corn {
    /**
     * @class EntityManager
     * @brief Manages all Entities in a scene
     *
     * The Entity Manager follows a factory pattern for managing the lifetime of Entities. It stores the Entities in
     * a tree structure. However, the root of the tree does not exist and cannot be accessed or destroyed.
     *
     * @see Entity
     * @see Scene
     */
    class EntityManager {
    public:
        // Entity needs access to the destroyEntity function
        friend class Entity;

        /**
         * @struct Node
         * @brief Tree node containing each Entity.
         */
        struct Node {
            Entity* ent;                           ///< Entity stored in the node
            Node* parent;                          ///< Parent node
            std::vector<Node*> children;           ///< Child nodes
            /**
             * @brief Whether the node's children are sorted by their z-order (small to large)
             *
             * False means it must be sorted, and true means it might not be.
             */
            bool dirty;
            Node(Entity* ent, Node* parent) noexcept;
        };

        /// @brief Constructor.
        explicit EntityManager(Scene& scene) noexcept;

        /// @brief Destructor.
        ~EntityManager();

        EntityManager(const EntityManager& other) = delete;
        EntityManager& operator=(const EntityManager& other) = delete;

        /// @return The scene that owns this Entity manager.
        [[nodiscard]] Scene& getScene() const noexcept;

        /// @return The game that contains this Entity manager.
        [[nodiscard]] const Game* getGame() const noexcept;

        /// @return The root node of the Entity tree.
        [[nodiscard]] const Node* getRoot() const noexcept;

        /**
         * @brief Creates a new entity with no components attached.
         * @param name Name of the entity. Entities can have the same name.
         * @param parent Parent entity to attach the new entity. If value is null, will attach to the root.
         * @return Pointer to the entity created.
         * @throw std::invalid_argument if parent is not a valid entity created by the entity manager.
         *
         * If reaches the maximum entity existing simultaneously, will result in undefined behavior.
         */
        Entity& createEntity(const std::string& name, const Entity* parent = nullptr);

        /**
         * @param id ID of the entity.
         * @return Entity with the given ID, or null pointer if it doesn't exist.
         *
         * Acquiring the entity by ID is the only method to access an entity in O(1) time complexity. All other methods
         * require traversing the entity tree, which takes O(n) time.
         */
        [[nodiscard]] Entity* getEntityByID(Entity::EntityID id) const noexcept;

        /**
         * @param name Name of the entity.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return The first entity with the given name, or null pointer if it doesn't exist.
         */
        [[nodiscard]] Entity* getEntityByName(const std::string& name, const Entity* parent = nullptr, bool recurse = true) const noexcept;

        /**
         * @param name Name of the entity.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All entities with the given name.
         */
        [[nodiscard]] std::vector<Entity*> getEntitiesByName(
                const std::string& name, const Entity* parent = nullptr, bool recurse = true) const noexcept;

        /**
         * @param pred A predicate function that takes an entity pointer and returns whether it satisfy the conditions.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return The first entity that satisfy the conditions given by filter, or null pointer if it doesn't exist.
         */
        [[nodiscard]] Entity* getEntityThat(
                const std::function<bool(const Entity*)>& pred, const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @param pred A predicate function that takes an entity pointer and returns whether it satisfy the conditions.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All entities that satisfy the conditions given by filter.
         */
        [[nodiscard]] std::vector<Entity*> getEntitiesThat(
                const std::function<bool(const Entity*)>& pred, const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All entities.
         */
        [[nodiscard]] std::vector<Entity*> getAllEntities(const Entity* parent = nullptr, bool recurse = true) const noexcept;

        /**
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All active entities. See `Entity::isActive()` for definition of active.
         */
        [[nodiscard]] std::vector<Entity*> getAllActiveEntities(const Entity* parent = nullptr, bool recurse = true) const noexcept;

        /**
         * @tparam T List of type of the components, must derive from Component class.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All entities with the list of components.
         */
        template <ComponentType... T>
        [[nodiscard]] std::vector<Entity*> getEntitiesWith(const Entity* parent = nullptr, bool recurse = true) const noexcept;

        /**
         * @tparam T List of type of the components, must derive from Component class.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All active entities with the list of components. See `Entity::isActive()` for definition of active.
         */
        template <ComponentType... T>
        [[nodiscard]] std::vector<Entity*> getActiveEntitiesWith(const Entity* parent = nullptr, bool recurse = true) const noexcept;

        /// @return A list of cameras components registered in this scene.
        [[nodiscard]] const std::vector<const CCamera*>& getCameras() const noexcept;

        /// @brief Cleans up all dirty nodes. Auto-called before rendering.
        void tidy() noexcept;

    private:
        /**
         * @brief Helper to `EntityManager::destroyEntity`
         *
         * Destroys a node and the entity inside, as well as all descendant nodes, but does not modify parent node.
         */
        void destroyNode(Node* node) noexcept;

        /**
         * @brief Destroys an entity. First destroys all children before destroying itself.
         * @param entity The entity to be destroyed.
         *
         * You should not use this function to destroy an entity. Use `entity.destroy()` instead.
         */
        void destroyEntity(Entity& entity) noexcept;

        /**
         * @defgroup Given a pointer to entity, return the node containing it.
         * @throw std::invalid_argument if parent is not a valid entity created by the entity manager.
         *
         * The two functions are the same, but one is the const version of the other.
         */
        /// @{
        const Node* getNodeFromEntity(const Entity* entity) const;
        Node* getNodeFromEntity(const Entity* entity);
        /// @}

        /**
         * @brief Helper to all getEntity/getEntities functions.
         * @param pred A predicate function that takes an entity pointer and returns whether it satisfy the conditions.
         *             Set it to null pointer to disable it.
         * @param onlyActive Whether to only consider active entities. See `Entity::isActiveInWorld()` for definition
         *                   of active.
         * @param limit Maximum number of entities to match. If set to 0, will match as much as possible.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All entities satisfying the given conditions.
         */
        [[nodiscard]] std::vector<Entity*> getEntitiesHelper(
                const std::function<bool(Entity*)>& pred, bool onlyActive, size_t limit,
                const Entity* parent, bool recurse) const;

        /// @brief The scene that owns this entity manager.
        Scene& scene_;

        /// @brief The root node (does not contain a entity).
        Node root_;

        /// @brief Quick access for finding nodes by entity ID (does not contain root).
        std::unordered_map<Entity::EntityID, Node> nodes_;

        /// @brief List of camera entities for quick access.
        std::vector<const CCamera*> cameras_;

        EventManager::ListenerID zOrderEventID_;
        EventManager::ListenerID cameraEventID_;
    };

    template<ComponentType... T>
    std::vector<Entity*> EntityManager::getEntitiesWith(const Entity* parent, bool recurse) const noexcept {
        return getEntitiesHelper([](Entity* entity) {
            return (... && entity->getComponent<T>());
        }, false, 0, parent, recurse);
    }

    template<ComponentType... T>
    std::vector<Entity*> EntityManager::getActiveEntitiesWith(const Entity* parent, bool recurse) const noexcept {
        return getEntitiesHelper([](Entity* entity) {
            return (... && entity->getComponent<T>());
        }, true, 0, parent, recurse);
    }
}

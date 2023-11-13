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
        friend class Interface;

        /// @brief The scene that owns the manager.
        Scene& scene;

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
             * False means it must be sorted, and true means it might not be
             */
            bool dirty;
            Node(Entity* ent, Node* parent);
        };

        explicit EntityManager(Scene& scene);
        ~EntityManager();
        EntityManager(const EntityManager& other) = delete;
        EntityManager& operator=(const EntityManager& other) = delete;

        /**
         * @brief Creates a new Entity with no Components attached.
         * @param name Name of the Entity. Entities can have the same name.
         * @param parent Parent Entity to attach the new Entity. If value is null, will attach to the root.
         * @return Pointer to the Entity created.
         * @throw std::invalid_argument if parent is not a valid Entity created by the Entity Manager.
         *
         * If reaches the maximum Entity existing simultaneously, will result in undefined behavior.
         */
        Entity& createEntity(const std::string& name, const Entity* parent = nullptr);

        /// @return The root node of the Entity tree.
        const Node* getRoot() const;

        /**
         * @param id ID of the Entity.
         * @return Entity with the given ID. Null pointer if it doesn't exist.
         *
         * Acquiring the Entity by ID is the only method to access an Entity in O(1) time complexity. All other methods
         * require traversing the Entity tree, which takes O(n) time.
         */
        Entity* getEntityByID(Entity::EntityID id) const;

        /**
         * @param name Name of the Entity.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return The first Entity with the given name. Null pointer if it doesn't exist.
         */
        Entity* getEntityByName(const std::string& name, const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @param name Name of the Entity.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All Entities with the given name.
         */
        std::vector<Entity*> getEntitiesByName(
                const std::string& name, const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @param pred A predicate function that takes an Entity pointer and returns whether it satisfy the conditions.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return The first Entity that satisfy the conditions given by filter. Null pointer if it doesn't exist.
         */
        Entity* getEntityThat(
                const std::function<bool(Entity*)>& pred, const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @param pred A predicate function that takes an Entity pointer and returns whether it satisfy the conditions.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All Entities that satisfy the conditions given by filter.
         */
        std::vector<Entity*> getEntitiesThat(
                const std::function<bool(Entity*)>& pred, const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All Entities.
         */
        std::vector<Entity*> getAllEntities(const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All active Entities. See `Entity::isActive()` for definition of active.
         */
        std::vector<Entity*> getAllActiveEntities(const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @tparam T List of type of the Components, must derive from Component class.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All Entities with the list of Components.
         */
        template <ComponentType... T>
        std::vector<Entity*> getEntitiesWith(const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @tparam T List of type of the Components, must derive from Component class.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All active Entities with the list of Components. See `Entity::isActive()` for definition of active.
         */
        template <ComponentType... T>
        std::vector<Entity*> getActiveEntitiesWith(const Entity* parent = nullptr, bool recurse = true) const;

        /// @brief Cleans up all dirty nodes. Auto-called before rendering.
        void tidy();

    private:
        /**
         * @brief Helper to EntityManager::destroyEntity
         *
         * Destroys a node and the Entity inside, as well as all descendant nodes, but does not modify parent node
         */
        void destroyNode(Node* node);

        /**
         * @brief Destroys an Entity. First destroys all children before destroying itself.
         * @param entity The Entity to be destroyed.
         *
         * You should not use this function to destroy an Entity. Use `entity.destroy()` instead.
         */
        void destroyEntity(Entity& entity);

        /**
         * @brief Given a pointer to Entity, return the Node containing it.
         * @throw std::invalid_argument if parent is not a valid Entity created by the Entity Manager.
         *
         * The two functions are the same, but one is the const version of the other.
         */
        const Node* getNodeFromEntity(const Entity* entity) const;
        Node* getNodeFromEntity(const Entity* entity);

        /**
         * @brief Helper to all getEntity/getEntities functions.
         * @param pred A predicate function that takes an Entity pointer and returns whether it satisfy the conditions.
         *             Set it to null pointer to disable it.
         * @param onlyActive Whether to only consider active entities. See `Entity::isActive()` for definition of
         *                   active.
         * @param limit Maximum number of entities to match. If set to 0, will match as much as possible.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All entities satisfying the given conditions.
         */
        std::vector<Entity*> getEntitiesHelper(
                const std::function<bool(Entity*)>& pred, bool onlyActive, size_t limit,
                const Entity* parent, bool recurse) const;

        /// @brief The root node (does not contain an entity)
        Node root;
        /// @brief Quick access for finding nodes by entity ID (does not contain root)
        std::unordered_map<Entity::EntityID, Node> nodes;
        /// @brief List of camera entities for quick access
        std::vector<const CCamera*> cameras;

        /// @brief Event listeners
        std::vector<EventManager::ListenerID> eventIDs;
    };

    template<ComponentType... T>
    std::vector<Entity*> EntityManager::getEntitiesWith(const Entity* parent, bool recurse) const {
        return getEntitiesHelper([](Entity* entity) {
            return (... && entity->getComponent<T>());
        }, false, 0, parent, recurse);
    }

    template<ComponentType... T>
    std::vector<Entity*> EntityManager::getActiveEntitiesWith(const Entity* parent, bool recurse) const {
        return getEntitiesHelper([](Entity* entity) {
            return (... && entity->getComponent<T>());
        }, true, 0, parent, recurse);
    }
}

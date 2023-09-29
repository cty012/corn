#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <corn/ecs/entity.h>

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
            Entity* ent;                           // Entity stored in the node
            Node* parent;                          // Parent node
            std::vector<Node*> children;           // Child nodes
            /**
             * @brief Whether the node's children are sorted by their z-order (small to large)
             *
             * False means it must be sorted, and true means it might not be
             */
            bool dirty;
            Node(Entity* ent, Node* parent);
        };

        EntityManager();
        ~EntityManager();

        /**
         * @brief Creates a new Entity with no Components attached.
         * @param name Name of the Entity. Entities can have the same name.
         * @param parent Parent Entity to attach the new Entity. If value is null, will attach to the root.
         * @return Reference to the Entity created.
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
         * @return The first entity with the given name. Null pointer if it doesn't exist.
         */
        Entity* getEntityByName(const std::string& name, const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @param name Name of the Entity.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All entities with the given name.
         */
        std::vector<Entity*> getEntitiesByName(const std::string& name, const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @param filter A function that takes an Entity pointer and returns whether it satisfy the conditions.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return The first entity that satisfy the conditions given by filter. Null pointer if it doesn't exist.
         */
        Entity* getEntityThat(const std::function<bool(Entity*)>& filter, const Entity* parent = nullptr, bool recurse = true) const;

        /**
         * @param name A function that takes an Entity pointer and returns whether it satisfy the conditions.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All entities that satisfy the conditions given by filter.
         */
        std::vector<Entity*> getEntitiesThat(
                const std::function<bool(Entity*)>& filter, const Entity* parent = nullptr, bool recurse = true) const;

        std::vector<Entity*> getActiveEntities();

    private:
        /// Helper to EntityManager::destroyEntity
        /// Destroys a node and the entity inside, as well as children, but does not modify parent node
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

        /// @brief The root node (does not contain a entity)
        Node root;
        /// @brief Quick access for finding nodes by entity ID (does not contain root)
        std::unordered_map<Entity::EntityID, Node> nodes;
    };
}

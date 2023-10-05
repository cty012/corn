#include <algorithm>
#include <stack>
#include <corn/ecs/entity_manager.h>
#include <corn/util/exceptions.h>

namespace corn {
    EntityManager::EntityManager()
        : root(Node(nullptr, nullptr)), nodes(std::unordered_map<Entity::EntityID, Node>()) {}

    EntityManager::~EntityManager() {
        for (auto& [id, node] : this->nodes) {
            delete node.ent;
        }
    }

    EntityManager::Node::Node(Entity* ent, Node* parent)
        : ent(ent), parent(parent), children(std::vector<Node*>()), dirty(false) {}

    Entity& EntityManager::createEntity(const std::string& name, const Entity* parent) {
        // Verify parent
        Node* parentNode = this->getNodeFromEntity(parent);

        // Create the entity
        static Entity::EntityID entID = 0;
        while (this->nodes.contains(entID)) {
            entID++;
        }
        auto* entity = new Entity(entID++, name, *this);

        // Create the node
        this->nodes.emplace(entity->id, Node(entity, parentNode));
        parentNode->children.push_back(&this->nodes.at(entity->id));
        parentNode->dirty = true;

        return *entity;
    }

    void EntityManager::destroyNode(Node* node) {
        if (node == nullptr) return;
        // Destroy all children
        for (Node* child : node->children) {
            EntityManager::destroyNode(child);
        }
        // Destroy self
        Entity::EntityID entID = node->ent->id;
        delete node->ent;
        this->nodes.erase(entID);
    }

    void EntityManager::destroyEntity(Entity& entity) {
        Node* node = &this->nodes.at(entity.id);
        Node* parent = node->parent;
        destroyNode(node);
        // Removes relation (parent --> node)
        parent->children.erase(
                std::remove(parent->children.begin(), parent->children.end(), node),
                parent->children.end());
    }

    const EntityManager::Node* EntityManager::getRoot() const {
        return &root;
    }

    Entity* EntityManager::getEntityByID(Entity::EntityID id) const {
        if (!this->nodes.contains(id)) return nullptr;
        return this->nodes.at(id).ent;
    }

    Entity* EntityManager::getEntityByName(const std::string& name, const Entity* parent, bool recurse) const {
        std::vector<Entity*> result = getEntitiesHelper([&name](Entity* entity) {
                return entity->name == name;
            }, false, 0, parent, recurse);
        return result.empty() ? nullptr : result[0];
    }

    std::vector<Entity*> EntityManager::getEntitiesByName(
            const std::string& name, const Entity* parent, bool recurse) const {

        return getEntitiesHelper([&name](Entity* entity) {
                return entity->name == name;
            }, false, 0, parent, recurse);
    }

    Entity* EntityManager::getEntityThat(
            const std::function<bool(Entity*)>& pred, const Entity* parent, bool recurse) const {

        std::vector<Entity*> result = getEntitiesHelper(pred, false, 0, parent, recurse);
        return result.empty() ? nullptr : result[0];
    }

    std::vector<Entity*> EntityManager::getEntitiesThat(
            const std::function<bool(Entity*)>& pred, const Entity* parent, bool recurse) const {

        return getEntitiesHelper(pred, false, 0, parent, recurse);
    }

    std::vector<Entity*> EntityManager::getAllEntities(const Entity* parent, bool recurse) const {
        return getEntitiesHelper(nullptr, false, 0, parent, recurse);
    }

    std::vector<Entity*> EntityManager::getAllActiveEntities(const Entity* parent, bool recurse) const {
        return getEntitiesHelper(nullptr, true, 0, parent, recurse);
    }

    void EntityManager::tidy() {
        auto nodeStack = std::stack<Node*>();
        nodeStack.push(&this->root);
        while (!nodeStack.empty()) {
            // Retrieve the next node
            Node* next = nodeStack.top();
            nodeStack.pop();

            // Skip if not active
            if ((next != &root) && !next->ent->active) continue;

            // Sort if dirty
            if (next->dirty) {
                next->dirty = false;
                std::stable_sort(next->children.begin(), next->children.end(),
                                 [](Node* left, Node* right) {
                                     auto ltrans = left->ent->getComponent<CTransform2D>();
                                     auto rtrans = right->ent->getComponent<CTransform2D>();
                                     if (rtrans == nullptr) return false;
                                     else if (ltrans == nullptr) return true;
                                     else return ltrans->zorder < rtrans->zorder;
                                 });
            }

            // Add children to stack
            std::for_each(next->children.rbegin(), next->children.rend(), [&nodeStack](Node* child) {
                nodeStack.push(child);
            });
        }
    }

    const EntityManager::Node* EntityManager::getNodeFromEntity(const Entity* entity) const {
        if (!entity) {
            return &this->root;
        } else if (&entity->entityManager == this) {
            return &this->nodes.at(entity->id);
        } else {
            throw std::invalid_argument("Parent Entity must be created by the same Entity Manager.");
        }
    }

    EntityManager::Node* EntityManager::getNodeFromEntity(const Entity* entity) {
        return const_cast<EntityManager::Node*>(static_cast<const EntityManager*>(this)->getNodeFromEntity(entity));
    }

    std::vector<Entity*> EntityManager::getEntitiesHelper(
            const std::function<bool(Entity*)> &pred, bool onlyActive,
            size_t limit, const Entity *parent, bool recurse) const {

        auto entities = std::vector<Entity*>();
        auto nodeStack = std::stack<const Node*>();
        const Node* parentNode = this->getNodeFromEntity(parent);
        std::for_each(parentNode->children.rbegin(), parentNode->children.rend(), [&nodeStack](Node *child) {
            nodeStack.push(child);
        });
        while (!nodeStack.empty()) {
            // Retrieve the next node
            const Node* next = nodeStack.top();
            nodeStack.pop();

            // Skip if not active
            if (onlyActive && (next != &root) && !next->ent->active) continue;

            // Check if current Entity satisfy conditions
            if (pred && pred(next->ent)) {
                entities.push_back(next->ent);
                if ((--limit) == 0) break;
            }

            // Add Entity pointer to vector and children to stack
            if (recurse) {
                std::for_each(next->children.rbegin(), next->children.rend(), [&nodeStack](Node *child) {
                    nodeStack.push(child);
                });
            }
        }
        return entities;
    }
}

#include <algorithm>
#include <stack>
#include <stdexcept>
#include <corn/ecs/entity_manager.h>

namespace corn {
    EntityManager::EntityManager() : root(Node(nullptr, nullptr)) {}

    EntityManager::~EntityManager() {
        for (auto& [id, node] : this->nodes) {
            delete node.ent;
        }
    }

    EntityManager::Node::Node(Entity* ent, Node* parent)
        : ent(ent), parent(parent), children(std::vector<Node*>()), dirty(false) {}

    Entity& EntityManager::createEntity(const std::string& name, const Entity* parent) {
        // Verify parent
        Node* parentNode = &this->root;
        if (parent != nullptr) {
            if (&parent->entityManager != this)
                throw std::invalid_argument("Parent Entity must be created by the same Entity Manager.");
            parentNode = &this->nodes.at(parent->id());
        }

        // Create the entity
        Entity* entity = nullptr;
        while (true) {
            entity = new Entity(name, *this);
            if (!this->nodes.contains(entity->id())) break;
            delete entity;
        }

        // Create the node
        this->nodes.emplace(entity->id(), Node(entity, parentNode));
        parentNode->children.push_back(&this->nodes.at(entity->id()));
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
        Entity::EntityID entID = node->ent->id();
        delete node->ent;
        this->nodes.erase(entID);
    }

    void EntityManager::destroyEntity(Entity& entity) {
        Node* node = &this->nodes.at(entity.id());
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

    std::vector<Entity*> EntityManager::getActiveEntities() {
        auto nodeStack = std::stack<Node*>();
        auto entities = std::vector<Entity*>();
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

            // Add Entity pointer to vector and children to stack
            if (next != &root) entities.push_back(next->ent);
            std::for_each(next->children.rbegin(), next->children.rend(), [&nodeStack](Node* child) {
                nodeStack.push(child);
            });
        }
        return entities;
    }
}

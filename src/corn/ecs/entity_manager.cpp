#include <algorithm>
#include <stdexcept>
#include <corn/ecs/entity_manager.h>

namespace corn {
    EntityManager::EntityManager() : root(Node(nullptr, nullptr)) {}

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
}

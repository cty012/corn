#include <utility>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>

namespace corn {
    Entity::Entity(EntityID id, std::string name, EntityManager& entityManager)
        : active(true), id(id), name(std::move(name)), entityManager(entityManager),
          components(std::unordered_map<std::type_index, Component*>()) {}

    Entity::~Entity() {
        // Destroy all components
        for (auto& [key, component] : this->components) {
            delete component;
            component = nullptr;
        }
    }

    void Entity::destroy() {
        this->entityManager.destroyEntity(*this);
    }

    Entity* Entity::getParent() const {
        EntityManager::Node* parent = this->entityManager.nodes.at(id).parent;
        if (!parent) return nullptr;
        return parent->ent;
    }

    std::vector<Entity*> Entity::getChildren() const {
        std::vector<EntityManager::Node*> children = this->entityManager.nodes.at(id).children;
        std::vector<Entity*> result = std::vector<Entity*>(children.size());
        for (size_t i = 0; i < children.size(); i++) {
            result[i] = children[i]->ent;
        }
        return result;
    }
}

#include <utility>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>

namespace corn {
    Entity::Entity(EntityID id, std::string name, EntityManager& entityManager)
            : id_(id), name_(std::move(name)), active_(true), entityManager_(entityManager), components_() {}

    Entity::~Entity() {
        // Destroy all components
        for (auto& [key, component] : this->components_) {
            delete component;
            component = nullptr;
        }
    }

    Entity::EntityID Entity::getID() const {
        return this->id_;
    }

    const std::string& Entity::getName() const {
        return this->name_;
    }

    void Entity::setName(std::string name) {
        this->name_ = std::move(name);
    }

    bool Entity::isActiveA() const {
        return this->active_;
    }

    void Entity::setActive(bool active) {
        this->active_ = active;
    }

    bool Entity::isActiveInWorld() const {
        const Entity* current = this;
        while (current) {
            if (!this->active_) return false;
            current = current->getParent();
        }
        return true;
    }

    EntityManager& Entity::getEntityManager() const {
        return this->entityManager_;
    }

    Scene& Entity::getScene() const {
        return this->entityManager_.getScene();
    }

    const Game* Entity::getGame() const {
        return this->entityManager_.getGame();
    }

    void Entity::destroy() {
        this->entityManager_.destroyEntity(*this);
    }

    Entity* Entity::getParent() const {
        EntityManager::Node* parent = this->entityManager_.nodes_.at(this->id_).parent;
        return parent ? parent->ent : nullptr;
    }

    std::vector<Entity*> Entity::getChildren() const {
        std::vector<EntityManager::Node*> children = this->entityManager_.nodes_.at(this->id_).children;
        std::vector<Entity*> result = std::vector<Entity*>(children.size());
        for (size_t i = 0; i < children.size(); i++) {
            result[i] = children[i]->ent;
        }
        return result;
    }
}

#include <utility>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>

namespace corn {
    Entity::Entity(std::string name, EntityManager& entityManager)
        : name(std::move(name)), entityManager(entityManager),
          components(std::unordered_map<std::type_index, Component*>()) {

        static EntityID uid = 0;
        this->uniqueId = uid++;
    }

    Entity::~Entity() {
        // Destroy all components
        for (auto& [key, component] : this->components) {
            delete component;
            component = nullptr;
        }
    }

    Entity::EntityID Entity::id() const {
        return this->uniqueId;
    }

    void Entity::destroy() {
        this->entityManager.destroyEntity(*this);
    }
}

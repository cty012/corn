#include <corn/ecs.h>


namespace corn {
    Entity::Entity(): components(std::vector<Component*>()) {
        static unsigned int uniqueId = 0;
        this->uniqueId = uniqueId++;
    }

    Entity::~Entity() {
        // TODO
    }

    EntityManager::EntityManager()
        : entities(std::vector<Entity>()) {}
}

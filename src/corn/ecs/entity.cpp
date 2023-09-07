#include <corn/ecs.h>


namespace corn {
    Entity::Entity()
        : components(std::vector<Component*>()) {
        static unsigned int uniqueId = 0;
        this->uniqueId = uniqueId++;
    }


    EntityManager::EntityManager()
        : entities(std::vector<Entity>()) {}
}

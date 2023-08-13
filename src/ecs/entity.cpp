#include <ecs/entity.h>


namespace corn {
    Entity::Entity() components(std::vector<Component*>()) {
        static unsigned int uniqueId = 0;
        this->uniqueId = uniqueId++;
    }
}

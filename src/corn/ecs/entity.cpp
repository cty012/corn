#include <corn/ecs/entity.h>

namespace corn {
    Entity::Entity(): components(std::vector<Component*>()) {
        static unsigned long long int uid = 0;
        this->uniqueId = uid++;
    }

    Entity::~Entity() {
        // TODO
    }
}

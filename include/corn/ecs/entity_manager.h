#pragma once

#include <corn/ecs/entity.h>

namespace corn {
    class EntityManager {
    public:
        EntityManager();
        ~EntityManager() = default;
        // TODO: add/get/remove entities

    private:
        std::vector<Entity> entities;
    };
}

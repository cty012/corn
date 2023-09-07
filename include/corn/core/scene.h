#pragma once

#include <vector>
#include <corn/ecs/entity.h>
#include <corn/ecs/system.h>


namespace corn {
    class Scene {
    private:
        unsigned long long uniqueId;
        EntityManager entityManager;
        std::vector<System*> systems;

    public:
        Scene();
        ~Scene();
        void fixedUpdate();
    };
}

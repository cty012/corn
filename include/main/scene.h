#pragma once

#include <vector>
#include <ecs/entity.h>
#include <ecs/system.h>


namespace corn {
    class Scene {
    private:
        unsigned int uniqueId;
        EntityManager entityManager;
        std::vector<System*> systems;

    public:
        Scene();
        ~Scene();
        void fixedUpdate();
    };
}

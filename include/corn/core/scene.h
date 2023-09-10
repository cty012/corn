#pragma once

#include <vector>
#include <corn/ecs/entity.h>
#include <corn/ecs/system.h>


namespace corn {
    enum class SceneOperation {
        PUSH, POP, REPLACE, REPLACE_ALL,
    };

    class Scene {
    public:
        Scene();
        ~Scene();
        void update();

    private:
        unsigned long long uniqueId;
        EntityManager entityManager;
        std::vector<System*> systems;
    };
}

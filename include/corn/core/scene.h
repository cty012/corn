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
        virtual ~Scene();
        void update();

    protected:
        EntityManager entityManager;

    private:
        std::vector<System*> systems;
    };
}

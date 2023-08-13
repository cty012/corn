#pragma once

#include <stack>
#include <corn/core/scene.h>


namespace corn {
    class Game {
    private:
        std::stack<Scene*> scenes;

    public:
        Game();
        ~Game();

        void addScene(Scene *scene);
        void run();
    };
}

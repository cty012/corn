#pragma once

#include <stack>
#include <corn/core/display.h>
#include <corn/core/scene.h>
#include <corn/event/event_manager.h>
#include <corn/util/config.h>

namespace corn {
    class Game {
    public:
        explicit Game(Scene* startScene, Config* config = nullptr);
        ~Game();

        /**
         * Change the current active scene. Only the top scene on the scene stack will be active. Note that the
         * scene pointer's deletion will be handled by the Game class.
         * @param op Operation on the scene
         * @param scene New scene to add. For POP operation, if a valid scene is passed in, then the scene will
         *  be deleted immediately.
         */
        void changeScene(SceneOperation op, Scene *scene);
        int run();

    private:
        void onSceneEvent(const EventArgsScene& args);
        bool removeOneScene();
        size_t removeAllScenes();

        /**
         * Game settings (e.g. resolution, display mode, volume, etc.).
         */
        Config* config;
        std::stack<Scene*> scenes;
        EventManager::ListenerID sceneEventId;
        Display* display;
    };
}

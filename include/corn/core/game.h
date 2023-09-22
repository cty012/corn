#pragma once

#include <stack>
#include <corn/core/interface.h>
#include <corn/core/scene.h>
#include <corn/event/event_manager.h>
#include <corn/util/config.h>

namespace corn {
    class Game {
    public:
        explicit Game(Scene* startScene, Config config = Config());
        ~Game();

        /**
         * @brief Get a const reference to the game's config data.
         * @return Config data of the game object
         */
        [[nodiscard]] const Config& getConfig() const;

        /// @brief Update the game's config data.
        void setConfig(Config newConfig);

        /**
         * @brief Change the current active scene.
         * @param op Operation on the scene
         * @param scene New scene to add. For POP operation, if a valid scene is passed in, then the scene will
         * be deleted immediately.
         *
         * Only the top scene on the scene stack will be active. Note that the scene pointer's deallocation will be
         * handled by the Game class (Scene must be on the heap).
         */
        void changeScene(SceneOperation op, Scene *scene);
        int run();

    private:
        bool removeOneScene();
        size_t removeAllScenes();

        void onSceneEvent(const EventArgsScene& args);
        void onCloseEvent(const EventArgsClose &args);

        /**
         * Game settings (e.g. resolution, display mode, volume, etc.).
         */
        bool active;
        Config config;
        std::stack<Scene*> scenes;
        EventManager::ListenerID sceneEventId;
        EventManager::ListenerID closeEventId;
        Interface* interface;
    };
}

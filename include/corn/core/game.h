#pragma once

#include <stack>
#include <corn/core/scene.h>
#include <corn/event/event_manager.h>
#include <corn/media/interface.h>
#include <corn/util/config.h>

namespace corn {
    /**
     * @class Game
     * @brief The main class that holds all components of the game.
     *
     * The Game class represents the "game" itself. It mainly consists of a scene stack and a interface for interacting
     * with hardware.
     *
     * The scene stack, as the name suggest, is a stack of scenes. Each scene object has its own set of ECS
     * architecture. The top scene is the only active scene, i.e. its data will be updated and displayed in the
     * window. When the scene stack is empty the game ends.
     *
     * @see Scene
     * @see Interface
     * @see Config
     */
    class Game {
    public:
        /**
         * @brief Constructor of the Game class.
         * @param startScene Starting scene on the scene stack.
         * @param config Game's config data.
         */
        explicit Game(Scene* startScene, Config config = Config());
        ~Game();

        /// @return Config data of the game object.
        [[nodiscard]] const Config& getConfig() const;

        /**
         * @brief Update the game's config data.
         * @param newConfig New configuration data to apply to the game.
         */
        void setConfig(Config newConfig);

        /**
         * @brief Launch the game.
         * @return Status code. 0 for success, non-zero for failure.
         *
         * Once this method is called, the game will enter the main loop until a exit event is received or the scene
         * stack is empty.
         */
        int run();

    private:
        bool removeOneScene();
        size_t removeAllScenes();

        /**
         * @brief Change the current active scene.
         * @param op Operation on the scene.
         * @param scene New scene to add.
         *
         * You should not call this method directly!!! Emit an event for safer scene stack manipulation.
         *
         * Only the top scene on the scene stack will be active. Note that the scene pointer's deallocation will be
         * handled by the Game class (scene must be on the heap).
         *
         * If the function is called with a POP operation and a valid scene pointer, the scene argument passed to the
         * function will NOT be deleted.
         */
        void changeScene(SceneOperation op, Scene* scene);

        /// @brief Callback to the scene event.
        void onSceneEvent(const EventArgsScene& args);

        /// @brief Callback to the exit event.
        void onExitEvent(const EventArgsExit &args);

        /// @brief Resolves all pending scene events at the end of a game loop.
        void resolveSceneEvents();

        /// Indicates if the game is currently running
        bool active;
        /// Game settings (e.g. resolution, display mode, volume, etc.)
        Config config;
        /// Scene stack
        std::stack<Scene*> scenes;
        std::queue<EventArgsScene> sceneEvents;
        Interface* interface;

        Stopwatch sw;

        EventManager::ListenerID sceneEventID;
        EventManager::ListenerID closeEventID;
    };
}

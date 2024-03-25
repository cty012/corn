#pragma once

#include <queue>
#include <stack>
#include <unordered_map>
#include <corn/event/event_args.h>
#include <corn/event/event_manager.h>
#include <corn/util/config.h>
#include <corn/util/stopwatch.h>

namespace corn {
    enum class Key;
    enum class SceneOperation;

    class Scene;
    class Interface;

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
    class Game final {
    public:
        /**
         * @brief Constructor.
         * @param startScene Starting scene on the scene stack. Memory managed by the Game class.
         * @param config Game's config data. Uses default values if not provided.
         */
        explicit Game(Scene* startScene, Config config = Config());

        /// @brief Destructor.
        ~Game();

        Game(const Game& other) = delete;
        Game& operator=(const Game& other) = delete;

        /// @return Config data of the game object.
        [[nodiscard]] const Config& getConfig() const noexcept;

        /**
         * @brief Updates the game's config data.
         * @param config New configuration data to apply to the game.
         */
        void setConfig(Config config);

        /**
         * @brief Get the currently active scene (the scene on the top of the scene stack).
         * @return The currently active scene if not empty, otherwise null pointer.
         */
        [[nodiscard]] Scene* getTopScene() const noexcept;

        /// @return An unordered map indicating which keys are currently pressed down.
        [[nodiscard]] const std::unordered_map<Key, bool>& getKeyPressed() const noexcept;

        /**
         * @brief Launch the game.
         *
         * Once this method is called, the game will enter the main loop until a exit event is received or the scene
         * stack is empty.
         */
        void run();

    private:
        /**
         * @brief Removes the scene at the top of the stack.
         * @return Whether the scene is removed successfully.
         *
         * Will return false if the scene stack is empty.
         */
        bool removeOneScene() noexcept;

        /**
         * @brief Clears the scene stack.
         * @return Number of scenes removed.
         */
        size_t removeAllScenes() noexcept;

        /**
         * @brief Change the current active scene.
         * @param op Operation on the scene.
         * @param scene New scene to add.
         *
         * You should not call this method directly. Emit a scene event for safer scene stack manipulation.
         *
         * Only the top scene on the scene stack will be active. Note that the scene pointer's deallocation will be
         * handled by the Game class (scene must be on the heap).
         *
         * If the function is called with a POP operation and a valid scene pointer, the scene argument passed to the
         * function will be deleted.
         */
        void changeScene(SceneOperation op, Scene* scene) noexcept;

        /// @brief Resolves all pending scene events at the end of a game loop.
        void resolveSceneEvents() noexcept;

        /// @brief Indicates if the game is currently running.
        bool active_;

        /**
         * @brief Game settings.
         * @example resolution, display mode, volume, etc.
         */
        Config config_;

        /// @brief Scene stack.
        std::stack<Scene*> scenes_;

        /// @brief Scene events to be processed.
        std::queue<EventArgsScene> sceneEvents_;

        /// @brief Module for rendering and capturing user input.
        Interface* interface_;

        /// @brief Stores which keys are currently pressed down.
        std::unordered_map<Key, bool> keyPressed_;

        /// @brief Stopwatch for timing between each frame.
        Stopwatch sw_;

        EventManager::ListenerID sceneEventID_;
        EventManager::ListenerID closeEventID_;
    };
}

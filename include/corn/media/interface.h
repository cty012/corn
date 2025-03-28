#pragma once

#include <unordered_map>

namespace corn {
    enum class Key;

    struct CCamera;
    class Game;
    class Scene;
    class UIManager;

    /**
     * @class Interface
     * @brief Receives input from the user and renders entities and UI to the window.
     *
     * @see Game
     */
    class Interface {
    public:
        /// @brief Constructor.
        Interface(const Game& game, std::unordered_map<Key, bool>& keyPressed_);

        /// @brief Destructor.
        ~Interface();

        Interface(const Interface& other) = delete;
        Interface& operator=(const Interface& other) = delete;

        /// @brief Creates the window and initializes resources.
        void init();

        /// @return The current size of the window in pixels.
        [[nodiscard]] Vec2f windowSize() const noexcept;

        /**
         * @brief Handles user keyboard, mouse, and other inputs and emits a global event.
         *
         * Keyboard and mouse input will only be emitted to the top scene's event room. Other events (such as close
         * event) will be emitted to the root without propagation.
         */
        void handleUserInput() const;

        /// @brief Clears the contents on the window.
        void clear();

        /**
         * @brief Renders the contents of the scene to the window.
         * @param scene The scene to render.
         */
        void render(Scene* scene);

        /**
         * @brief Renders the debug overlay at a corner of the window.
         * @param fps The current frames per second.
         */
        void renderDebugOverlay(size_t fps);

        /// @brief Flushes all changes.
        void update();

    private:
        /**
         * @param camera The target camera component.
         * @return The transformation (offset and scale) that defines how to transform coordinates from the world's
         * reference frame to camera's reference frame.
         */
        [[nodiscard]] std::pair<Vec2f, Vec2f> getCameraTransformation(const CCamera* camera) const;

        /**
         * @brief Render the view of the camera onto a temporary texture.
         * @param scene The scene containing the target entities.
         * @param camera The target camera.
         */
        void renderCamera(Scene* scene, const CCamera* camera);

        /**
         * @brief Render the UI onto the window.
         * @param uiManager The target UI manager.
         */
        void renderUI(UIManager& uiManager);

        /// @brief The game that owns the interface.
        const Game& game_;

        /// @brief Reference to the map that stores the state of all keys.
        std::unordered_map<Key, bool>& keyPressed_;

        /// @brief Pimpl idiom.
        class InterfaceImpl;
        InterfaceImpl* impl_;
    };
}

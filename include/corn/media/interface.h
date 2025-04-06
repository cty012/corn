#pragma once

#include <unordered_map>
#include <corn/event/input.h>
#include <corn/geometry/transform.h>
#include <corn/geometry/vec.h>

namespace corn {
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
        class InterfaceImpl;

        /// @brief Constructor.
        explicit Interface(const Game& game);

        /// @brief Destructor.
        ~Interface();

        Interface(const Interface& other) = delete;
        Interface& operator=(const Interface& other) = delete;

        /// @brief Creates the window and initializes resources.
        void init();

        /// @return The current logical size of the window in pixels.
        [[nodiscard]] Vec2f windowLogicalSize() const noexcept;

        /// @return The current physical (framebuffer) size of the window in pixels.
        [[nodiscard]] Vec2f windowPhysicalSize() const noexcept;

        [[nodiscard]] float getHiDPIScale() const noexcept;

        /// @brief Checks if the window is resized.
        void checkWindowResize() const noexcept;

        /**
         * @brief Handles user keyboard, mouse, and other inputs and emits a global event.
         *
         * Keyboard and mouse input will only be emitted to the top scene's event room. Other events (such as close
         * event) will be emitted to the root without propagation.
         */
        void handleUserInput();

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

        /**
         * @param mouseButton The target mouse button.
         * @return
         */
        [[nodiscard]] bool isPressed(MouseButton mouseButton) const noexcept;

        /**
         * @param key The target key.
         * @return Whether the key is currently pressed down.
         */
        [[nodiscard]] bool isPressed(Key key) const noexcept;

    private:
        void handleMouseInput();
        void handleKeyboardInput();

        /**
         * @param camera The target camera component.
         * @return The transformation that defines how to transform coordinates from the camera's reference frame to the
         *         world's reference frame.
         */
        [[nodiscard]] Transform2D getCameraTransformation(const CCamera* camera) const;

        /**
         * @brief Render the view of the camera onto a temporary texture.
         * @param scene The scene containing the target entities.
         * @param camera The target camera.
         */
        void renderCamera2D(Scene* scene, const CCamera* camera);

        /**
         * @brief Render the UI onto the window.
         * @param uiManager The target UI manager.
         */
        void renderUI(UIManager& uiManager);

        /// @brief The game that owns the interface.
        const Game& game_;

        /// @brief Pimpl idiom.
        InterfaceImpl* impl_;
    };
}

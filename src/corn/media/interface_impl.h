#pragma once

#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "../render/shader.h"

namespace corn {
    class Interface::InterfaceImpl {
    public:
        GLFWwindow* window = nullptr;
        uint16_t width = 0, height = 0, fwidth = 0, fheight = 0;

        Shader polygonShader, bitmapShader;

        // Temp variable
        bgfx::ViewId viewID = 0;

        /// @brief Whether the window is currently focused.
        bool focused = true;

        /// @brief Whether the mouse is currently inside the window.
        bool mouseInside = true;

        /// @brief The current location of the mouse.
        Vec2f mousePosition;

        /// @brief The current scroll value of the mouse (x: horizontal, y: vertical).
        Vec2f mouseScroll;

        /// @brief Stores the current state of all mouse buttons.
        std::unordered_map<MouseButton, bool> mousePressed;

        /// @brief Stores the current state of all keys.
        std::unordered_map<Key, bool> keyPressed;

        /// @brief The current input string.
        std::string input;
    };
}

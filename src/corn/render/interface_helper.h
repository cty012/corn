#pragma once

#include <GLFW/glfw3.h>
#include <corn/ecs/component.h>
#include <corn/event/input.h>
#include <corn/media/interface.h>
#include <corn/ui/ui_label.h>
#include <corn/util/config.h>
#include "shader.h"

namespace corn {
    // Draw components
    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CSprite& cSprite,
            const Transform2D& cameraTransform);

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CLines& cLines,
            const Transform2D& cameraTransform);

    void draw(
            bgfx::ViewId viewID,
            const CTransform2D& cTransform, const CPolygon& cPolygon,
            const Transform2D& worldToCameraTransform, const Shader& polygonShader);

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CText& cText,
            const Transform2D& cameraTransform);

    void drawUI(
            bgfx::ViewId viewID,
            UILabel& uiLabel, float maxWidth,
            const Transform2D& transform, const Shader& bitmapShader);

    void drawDebug(
            bgfx::ViewId viewID,
            const RichTextFrame& debugText, float xPad, float yPad, float dpiScale,
            const Shader& polygonShader, const Shader& bitmapShader);

    // GLFW helper constants and functions
    constexpr std::array GLFW_CORN_MOUSE_BUTTONS = {
            std::pair{ GLFW_MOUSE_BUTTON_LEFT, MouseButton::LEFT },
            std::pair{ GLFW_MOUSE_BUTTON_RIGHT, MouseButton::RIGHT },
            std::pair{ GLFW_MOUSE_BUTTON_MIDDLE, MouseButton::MIDDLE },
    };

    constexpr std::array GLFW_CORN_KEYS = {
            std::pair{ GLFW_KEY_ESCAPE, Key::ESC },
            std::pair{ GLFW_KEY_F1, Key::F1 },
            std::pair{ GLFW_KEY_F2, Key::F2 },
            std::pair{ GLFW_KEY_F3, Key::F3 },
            std::pair{ GLFW_KEY_F4, Key::F4 },
            std::pair{ GLFW_KEY_F5, Key::F5 },
            std::pair{ GLFW_KEY_F6, Key::F6 },
            std::pair{ GLFW_KEY_F7, Key::F7 },
            std::pair{ GLFW_KEY_F8, Key::F8 },
            std::pair{ GLFW_KEY_F9, Key::F9 },
            std::pair{ GLFW_KEY_F10, Key::F10 },
            std::pair{ GLFW_KEY_F11, Key::F11 },
            std::pair{ GLFW_KEY_F12, Key::F12 },
            std::pair{ GLFW_KEY_SPACE, Key::SPACE },

            std::pair{ GLFW_KEY_GRAVE_ACCENT, Key::GRAVE },
            std::pair{ GLFW_KEY_1, Key::K_1 },
            std::pair{ GLFW_KEY_2, Key::K_2 },
            std::pair{ GLFW_KEY_3, Key::K_3 },
            std::pair{ GLFW_KEY_4, Key::K_4 },
            std::pair{ GLFW_KEY_5, Key::K_5 },
            std::pair{ GLFW_KEY_6, Key::K_6 },
            std::pair{ GLFW_KEY_7, Key::K_7 },
            std::pair{ GLFW_KEY_8, Key::K_8 },
            std::pair{ GLFW_KEY_9, Key::K_9 },
            std::pair{ GLFW_KEY_0, Key::K_0 },

            std::pair{ GLFW_KEY_A, Key::A },
            std::pair{ GLFW_KEY_B, Key::B },
            std::pair{ GLFW_KEY_C, Key::C },
            std::pair{ GLFW_KEY_D, Key::D },
            std::pair{ GLFW_KEY_E, Key::E },
            std::pair{ GLFW_KEY_F, Key::F },
            std::pair{ GLFW_KEY_G, Key::G },
            std::pair{ GLFW_KEY_H, Key::H },
            std::pair{ GLFW_KEY_I, Key::I },
            std::pair{ GLFW_KEY_J, Key::J },
            std::pair{ GLFW_KEY_K, Key::K },
            std::pair{ GLFW_KEY_L, Key::L },
            std::pair{ GLFW_KEY_M, Key::M },
            std::pair{ GLFW_KEY_N, Key::N },
            std::pair{ GLFW_KEY_O, Key::O },
            std::pair{ GLFW_KEY_P, Key::P },
            std::pair{ GLFW_KEY_Q, Key::Q },
            std::pair{ GLFW_KEY_R, Key::R },
            std::pair{ GLFW_KEY_S, Key::S },
            std::pair{ GLFW_KEY_T, Key::T },
            std::pair{ GLFW_KEY_U, Key::U },
            std::pair{ GLFW_KEY_V, Key::V },
            std::pair{ GLFW_KEY_W, Key::W },
            std::pair{ GLFW_KEY_X, Key::X },
            std::pair{ GLFW_KEY_Y, Key::Y },
            std::pair{ GLFW_KEY_Z, Key::Z },

            std::pair{ GLFW_KEY_TAB, Key::TAB },
            std::pair{ GLFW_KEY_CAPS_LOCK, Key::CAPS },
            std::pair{ GLFW_KEY_LEFT_SHIFT, Key::LSHIFT },
            std::pair{ GLFW_KEY_RIGHT_SHIFT, Key::RSHIFT },
            std::pair{ GLFW_KEY_LEFT_CONTROL, Key::LCTRL },
            std::pair{ GLFW_KEY_RIGHT_CONTROL, Key::RCTRL },
            std::pair{ GLFW_KEY_LEFT_ALT, Key::LALT },
            std::pair{ GLFW_KEY_RIGHT_ALT, Key::RALT },
            std::pair{ GLFW_KEY_LEFT_SUPER, Key::LSYS },
            std::pair{ GLFW_KEY_RIGHT_SUPER, Key::RSYS },
            std::pair{ GLFW_KEY_BACKSPACE, Key::BACKSPACE },
            std::pair{ GLFW_KEY_ENTER, Key::ENTER },

            std::pair{ GLFW_KEY_MINUS, Key::HYPHEN },
            std::pair{ GLFW_KEY_EQUAL, Key::EQUAL },
            std::pair{ GLFW_KEY_LEFT_BRACKET, Key::LBRACKET },
            std::pair{ GLFW_KEY_RIGHT_BRACKET, Key::RBRACKET },
            std::pair{ GLFW_KEY_SLASH, Key::SLASH },
            std::pair{ GLFW_KEY_BACKSLASH, Key::BACKSLASH },
            std::pair{ GLFW_KEY_COMMA, Key::COMMA },
            std::pair{ GLFW_KEY_PERIOD, Key::PERIOD },
            std::pair{ GLFW_KEY_SEMICOLON, Key::SEMICOLON },
            std::pair{ GLFW_KEY_APOSTROPHE, Key::APOSTROPHE },

            std::pair{ GLFW_KEY_UP, Key::UP },
            std::pair{ GLFW_KEY_DOWN, Key::DOWN },
            std::pair{ GLFW_KEY_LEFT, Key::LEFT },
            std::pair{ GLFW_KEY_RIGHT, Key::RIGHT },
            std::pair{ GLFW_KEY_HOME, Key::HOME },
            std::pair{ GLFW_KEY_END, Key::END },
            std::pair{ GLFW_KEY_INSERT, Key::INS },
            std::pair{ GLFW_KEY_DELETE, Key::DEL },
            std::pair{ GLFW_KEY_PAGE_UP, Key::PGUP },
            std::pair{ GLFW_KEY_PAGE_DOWN, Key::PGDOWN },

            std::pair{ GLFW_KEY_KP_0, Key::NUMPAD_0 },
            std::pair{ GLFW_KEY_KP_1, Key::NUMPAD_1 },
            std::pair{ GLFW_KEY_KP_2, Key::NUMPAD_2 },
            std::pair{ GLFW_KEY_KP_3, Key::NUMPAD_3 },
            std::pair{ GLFW_KEY_KP_4, Key::NUMPAD_4 },
            std::pair{ GLFW_KEY_KP_5, Key::NUMPAD_5 },
            std::pair{ GLFW_KEY_KP_6, Key::NUMPAD_6 },
            std::pair{ GLFW_KEY_KP_7, Key::NUMPAD_7 },
            std::pair{ GLFW_KEY_KP_8, Key::NUMPAD_8 },
            std::pair{ GLFW_KEY_KP_9, Key::NUMPAD_9 },

            std::pair{ GLFW_KEY_KP_ADD, Key::NUMPAD_ADD },
            std::pair{ GLFW_KEY_KP_SUBTRACT, Key::NUMPAD_SUB },
            std::pair{ GLFW_KEY_KP_MULTIPLY, Key::NUMPAD_MUL },
            std::pair{ GLFW_KEY_KP_DIVIDE, Key::NUMPAD_DIV },
            std::pair{ GLFW_KEY_KP_DECIMAL, Key::NUMPAD_DECIMAL },
            std::pair{ GLFW_KEY_KP_EQUAL, Key::NUMPAD_EQUAL },
            std::pair{ GLFW_KEY_KP_ENTER, Key::NUMPAD_ENTER },
    };

    inline uint8_t getModifiers(GLFWwindow* window) {
        uint8_t modifiers = 0;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
            modifiers |= 1 << 0;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
            modifiers |= 1 << 1;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) {
            modifiers |= 1 << 2;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS) {
            modifiers |= 1 << 3;
        }
        return modifiers;
    }

    inline size_t unicodeToUTF8(uint32_t codepoint, char* output) {
        if (codepoint <= 0x7F) {
            // 1-byte sequence
            output[0] = static_cast<char>(codepoint);
            return 1;
        } else if (codepoint <= 0x7FF) {
            // 2-byte sequence
            output[0] = static_cast<char>(0xC0 | (codepoint >> 6));
            output[1] = static_cast<char>(0x80 | (codepoint & 0x3F));
            return 2;
        } else if (codepoint <= 0xFFFF) {
            // 3-byte sequence
            output[0] = static_cast<char>(0xE0 | (codepoint >> 12));
            output[1] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            output[2] = static_cast<char>(0x80 | (codepoint & 0x3F));
            return 3;
        } else if (codepoint <= 0x10FFFF) {
            // 4-byte sequence
            output[0] = static_cast<char>(0xF0 | (codepoint >> 18));
            output[1] = static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
            output[2] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            output[3] = static_cast<char>(0x80 | (codepoint & 0x3F));
            return 4;
        } else {
            // Invalid Unicode code point
            return 0;
        }
    }
}

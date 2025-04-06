#pragma once

namespace corn {
    /**
     * @class Key
     * @brief Name of the keys on the keyboard.
     */
    enum class Key {
        ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, SPACE,
        GRAVE, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        TAB, CAPS, LSHIFT, RSHIFT, LCTRL, RCTRL, LALT, RALT, LSYS, RSYS, BACKSPACE, ENTER,
        HYPHEN, EQUAL, LBRACKET, RBRACKET, SLASH, BACKSLASH, COMMA, PERIOD, SEMICOLON, APOSTROPHE,
        UP, DOWN, LEFT, RIGHT, HOME, END, INS, DEL, PGUP, PGDOWN,
        NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4, NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9,
        NUMPAD_ADD, NUMPAD_SUB, NUMPAD_MUL, NUMPAD_DIV, NUMPAD_DECIMAL, NUMPAD_EQUAL, NUMPAD_ENTER,
        NONE,
    };

    constexpr uint8_t KEY_MODIFIER_NONE = 0;
    constexpr uint8_t KEY_MODIFIER_SHIFT = 1 << 0;
    constexpr uint8_t KEY_MODIFIER_CTRL = 1 << 1;
    constexpr uint8_t KEY_MODIFIER_ALT = 1 << 2;
    constexpr uint8_t KEY_MODIFIER_SYS = 1 << 3;

    /**
     * @class MouseButton
     * @brief Name of the mouse buttons.
     */
    enum class MouseButton {
        LEFT, RIGHT, MIDDLE, NONE
    };

    /**
     * @class ButtonEvent
     * @brief Standard interactions with a button.
     */
    enum class ButtonEvent {
        DOWN, UP,
    };
}

#pragma once

namespace corn {
    enum class Key {
        ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, SPACE,
        GRAVE, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        TAB, CAPS, LSHIFT, RSHIFT, LCTRL, RCTRL, LALT, RALT, LSYS, RSYS, BACKSPACE, ENTER,
        HYPHEN, EQUAL, LBRACKET, RBRACKET, SLASH, BSLASH, COMMA, PERIOD, SEMICOLON, APOSTROPHE,
        UP, DOWN, LEFT, RIGHT, HOME, END, INS, DEL, PGUP, PGDOWN,
        NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, NUM_ADD, NUM_SUB, NUM_MUL, NUM_DIV,
        NONE,
    };

    enum class Mouse {
        LEFT, RIGHT, MIDDLE, NONE
    };

    enum class ButtonEvent {
        DOWN, UP,
    };
}

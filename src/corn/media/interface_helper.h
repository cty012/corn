#pragma once

#include <SFML/Graphics.hpp>
#include <corn/ecs/component.h>
#include <corn/event/input.h>
#include <corn/geometry/vec2.h>
#include <corn/media/interface.h>
#include <corn/util/config.h>

namespace corn {
    // Draw components
    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CSprite& cSprite,
            const Vec2& cameraOffset, const Vec2& cameraScale, const sf::Transform& scaleTransform);

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CLines& cLines,
            const Vec2& cameraOffset, const Vec2& cameraScale, const sf::Transform& scaleTransform);

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CPolygon& cPolygon,
            const Vec2& cameraOffset, const Vec2& cameraScale, const sf::Transform& scaleTransform);

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CText& cText,
            const Vec2& cameraOffset, const Vec2& cameraScale, const sf::Transform& scaleTransform);

    // Inline functions
    inline DisplayMode sfStyle2CornMode(int style) {
        return style & 0b1000 ? DisplayMode::FULLSCREEN : DisplayMode::WINDOWED;
    }

    inline unsigned int cornMode2SfStyle(DisplayMode mode) {
        switch (mode) {
            case DisplayMode::WINDOWED:
                return sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
            case DisplayMode::WINDOWED_FIXED:
                return sf::Style::Titlebar | sf::Style::Close;
            case DisplayMode::FULLSCREEN:
                return sf::Style::Fullscreen;
            default:
                return 0b0101;
        }
    }

    inline Key sfInput2CornInput(sf::Keyboard::Key key, sf::Keyboard::Scancode code) {
        switch (key) {
            case sf::Keyboard::A:
                return Key::A;
            case sf::Keyboard::B:
                return Key::B;
            case sf::Keyboard::C:
                return Key::C;
            case sf::Keyboard::D:
                return Key::D;
            case sf::Keyboard::E:
                return Key::E;
            case sf::Keyboard::F:
                return Key::F;
            case sf::Keyboard::G:
                return Key::G;
            case sf::Keyboard::H:
                return Key::H;
            case sf::Keyboard::I:
                return Key::I;
            case sf::Keyboard::J:
                return Key::J;
            case sf::Keyboard::K:
                return Key::K;
            case sf::Keyboard::L:
                return Key::L;
            case sf::Keyboard::M:
                return Key::M;
            case sf::Keyboard::N:
                return Key::N;
            case sf::Keyboard::O:
                return Key::O;
            case sf::Keyboard::P:
                return Key::P;
            case sf::Keyboard::Q:
                return Key::Q;
            case sf::Keyboard::R:
                return Key::R;
            case sf::Keyboard::S:
                return Key::S;
            case sf::Keyboard::T:
                return Key::T;
            case sf::Keyboard::U:
                return Key::U;
            case sf::Keyboard::V:
                return Key::V;
            case sf::Keyboard::W:
                return Key::W;
            case sf::Keyboard::X:
                return Key::X;
            case sf::Keyboard::Y:
                return Key::Y;
            case sf::Keyboard::Z:
                return Key::Z;
            case sf::Keyboard::Num0:
                return Key::K_0;
            case sf::Keyboard::Num1:
                return Key::K_1;
            case sf::Keyboard::Num2:
                return Key::K_2;
            case sf::Keyboard::Num3:
                return Key::K_3;
            case sf::Keyboard::Num4:
                return Key::K_4;
            case sf::Keyboard::Num5:
                return Key::K_5;
            case sf::Keyboard::Num6:
                return Key::K_6;
            case sf::Keyboard::Num7:
                return Key::K_7;
            case sf::Keyboard::Num8:
                return Key::K_8;
            case sf::Keyboard::Num9:
                return Key::K_9;
            case sf::Keyboard::Escape:
                return Key::ESC;
            case sf::Keyboard::LControl:
                return Key::LCTRL;
            case sf::Keyboard::LShift:
                return Key::LSHIFT;
            case sf::Keyboard::LAlt:
                return Key::LALT;
            case sf::Keyboard::LSystem:
                return Key::LSYS;
            case sf::Keyboard::RControl:
                return Key::RCTRL;
            case sf::Keyboard::RShift:
                return Key::RSHIFT;
            case sf::Keyboard::RAlt:
                return Key::RALT;
            case sf::Keyboard::RSystem:
                return Key::RSYS;
            case sf::Keyboard::LBracket:
                return Key::LBRACKET;
            case sf::Keyboard::RBracket:
                return Key::RBRACKET;
            case sf::Keyboard::Semicolon:
                return Key::SEMICOLON;
            case sf::Keyboard::Comma:
                return Key::COMMA;
            case sf::Keyboard::Period:
                return Key::PERIOD;
            case sf::Keyboard::Apostrophe:
                return Key::APOSTROPHE;
            case sf::Keyboard::Slash:
                return Key::SLASH;
            case sf::Keyboard::Backslash:
                return Key::BSLASH;
            case sf::Keyboard::Grave:
                return Key::GRAVE;
            case sf::Keyboard::Equal:
                return Key::EQUAL;
            case sf::Keyboard::Hyphen:
                return Key::HYPHEN;
            case sf::Keyboard::Space:
                return Key::SPACE;
            case sf::Keyboard::Enter:
                return Key::ENTER;
            case sf::Keyboard::Backspace:
                return Key::BACKSPACE;
            case sf::Keyboard::Tab:
                return Key::TAB;
            case sf::Keyboard::PageUp:
                return Key::PGUP;
            case sf::Keyboard::PageDown:
                return Key::PGDOWN;
            case sf::Keyboard::End:
                return Key::END;
            case sf::Keyboard::Home:
                return Key::HOME;
            case sf::Keyboard::Insert:
                return Key::INS;
            case sf::Keyboard::Delete:
                return Key::DEL;
            case sf::Keyboard::Add:
                return Key::NUM_ADD;
            case sf::Keyboard::Subtract:
                return Key::NUM_SUB;
            case sf::Keyboard::Multiply:
                return Key::NUM_MUL;
            case sf::Keyboard::Divide:
                return Key::NUM_DIV;
            case sf::Keyboard::Left:
                return Key::LEFT;
            case sf::Keyboard::Right:
                return Key::RIGHT;
            case sf::Keyboard::Up:
                return Key::UP;
            case sf::Keyboard::Down:
                return Key::DOWN;
            case sf::Keyboard::Numpad0:
                return Key::NUM_0;
            case sf::Keyboard::Numpad1:
                return Key::NUM_1;
            case sf::Keyboard::Numpad2:
                return Key::NUM_2;
            case sf::Keyboard::Numpad3:
                return Key::NUM_3;
            case sf::Keyboard::Numpad4:
                return Key::NUM_4;
            case sf::Keyboard::Numpad5:
                return Key::NUM_5;
            case sf::Keyboard::Numpad6:
                return Key::NUM_6;
            case sf::Keyboard::Numpad7:
                return Key::NUM_7;
            case sf::Keyboard::Numpad8:
                return Key::NUM_8;
            case sf::Keyboard::Numpad9:
                return Key::NUM_9;
            case sf::Keyboard::F1:
                return Key::F1;
            case sf::Keyboard::F2:
                return Key::F2;
            case sf::Keyboard::F3:
                return Key::F3;
            case sf::Keyboard::F4:
                return Key::F4;
            case sf::Keyboard::F5:
                return Key::F5;
            case sf::Keyboard::F6:
                return Key::F6;
            case sf::Keyboard::F7:
                return Key::F7;
            case sf::Keyboard::F8:
                return Key::F8;
            case sf::Keyboard::F9:
                return Key::F9;
            case sf::Keyboard::F10:
                return Key::F10;
            case sf::Keyboard::F11:
                return Key::F11;
            case sf::Keyboard::F12:
                return Key::F12;
            case sf::Keyboard::Unknown:
                return code == sf::Keyboard::Scancode::CapsLock ? Key::CAPS : Key::NONE;
            default:
                return Key::NONE;
        }
    }

    inline Mouse sfInput2CornInput(sf::Mouse::Button button) {
        switch (button) {
            case sf::Mouse::Button::Left:
                return Mouse::LEFT;
            case sf::Mouse::Right:
                return Mouse::RIGHT;
            case sf::Mouse::Middle:
                return Mouse::MIDDLE;
            default:
                return Mouse::NONE;
        }
    }
}

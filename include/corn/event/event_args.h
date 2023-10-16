#pragma once

#include <string>
#include <corn/core/input.h>
#include <corn/geometry/vec2.h>

namespace corn {
    struct EventArgs {
        /// @return Type of the event
        [[nodiscard]] virtual constexpr const char* type() const = 0;
    };

    struct EventArgsExit : public EventArgs {
        [[nodiscard]] constexpr const char* type() const override { return "corn::input::exit"; }
    };

    struct EventArgsKeyboard : public EventArgs {
        [[nodiscard]] constexpr const char* type() const override { return "corn::input::keyboard"; }
        EventArgsKeyboard(Key key, ButtonEvent status, unsigned char modifiers, const Vec2& mousePos);
        Key key;
        ButtonEvent status;
        // modifiers & (1 << 3): SYS key
        // modifiers & (1 << 2): CTRL key
        // modifiers & (1 << 1): SHIFT key
        // modifiers & (1 << 0): ALT key
        unsigned char modifiers;
        Vec2 mousePos;
    };

    struct EventArgsMouseButton : public EventArgs {
        [[nodiscard]] constexpr const char* type() const override { return "corn::input::mousebtn"; }
        EventArgsMouseButton(Mouse mouse, ButtonEvent status, const Vec2& mousePos);
        Mouse mouse;
        ButtonEvent status;
        Vec2 mousePos;
    };

    struct EventArgsMouseMove : public EventArgs {
        [[nodiscard]] constexpr const char* type() const override { return "corn::input::mousemv"; }
        explicit EventArgsMouseMove(const Vec2& mousePos);
        Vec2 mousePos;
    };

    struct EventArgsMouseScroll : public EventArgs {
        [[nodiscard]] constexpr const char* type() const override { return "corn::input::mousesc"; }
        EventArgsMouseScroll(double value, const Vec2& mousePos);
        double value;
        Vec2 mousePos;
    };

    class Scene;
    enum class SceneOperation;

    struct EventArgsScene : public EventArgs {
        [[nodiscard]] constexpr const char* type() const override { return "corn::game::scene"; }
        EventArgsScene(SceneOperation op, Scene* scene);
        SceneOperation op;
        Scene* scene;
    };
}

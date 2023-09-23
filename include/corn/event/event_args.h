#pragma once

#include <string>
#include <corn/core/input.h>
#include <corn/util/geometry.h>

namespace corn {
    struct EventArgs {
        /// @return Type of the event
        [[nodiscard]] virtual std::string type() const = 0;
    };

    struct EventArgsExit : public EventArgs {
        [[nodiscard]] std::string type() const override;
    };

    struct EventArgsKeyboard : public EventArgs {
        explicit EventArgsKeyboard(Key key, KeyEvent keyEvent, unsigned char modifiers, const Vec2& mousePos);
        [[nodiscard]] std::string type() const override;
        Key key;
        KeyEvent keyEvent;
        // modifiers & (1 << 3): SYS key
        // modifiers & (1 << 2): CTRL key
        // modifiers & (1 << 1): SHIFT key
        // modifiers & (1 << 0): ALT key
        unsigned char modifiers;
        Vec2 mousePos;
    };

    struct EventArgsMouse : public EventArgs {
        explicit EventArgsMouse(MouseEvent mouseEvent, double value, const Vec2& mousePos);
        [[nodiscard]] std::string type() const override;
        MouseEvent mouseEvent;
        double value;
        Vec2 mousePos;
    };

    class Scene;
    enum class SceneOperation;

    struct EventArgsScene : public EventArgs {
        EventArgsScene(SceneOperation op, Scene* scene);
        [[nodiscard]] std::string type() const override;
        SceneOperation op;
        Scene* scene;
    };
}

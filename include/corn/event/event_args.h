#pragma once

#include <string>
#include <corn/core/input.h>
#include <corn/util/geometry.h>

namespace corn {
    class EventArgs {
    public:
        /**
         * @return Type of the event
         */
        [[nodiscard]] virtual std::string type() const = 0;
    };

    class EventArgsExit : public EventArgs {
    public:
        [[nodiscard]] std::string type() const override;
    };

    class EventArgsKeyboard : public EventArgs {
    public:
        explicit EventArgsKeyboard(Key key, KeyEvent keyEvent, unsigned char modifiers, const Vec2& mousePos);
        [[nodiscard]] std::string type() const override;
        [[nodiscard]] Key getKey() const;
        [[nodiscard]] KeyEvent getKeyEvent() const;
        [[nodiscard]] unsigned char getModifiers() const;
        [[nodiscard]] const Vec2& getMousePos() const;

    private:
        Key key;
        KeyEvent keyEvent;
        // modifiers & (1 << 3): SYS key
        // modifiers & (1 << 2): CTRL key
        // modifiers & (1 << 1): SHIFT key
        // modifiers & (1 << 0): ALT key
        unsigned char modifiers;
        Vec2 mousePos;
    };

    class EventArgsMouse : public EventArgs {
    public:
        explicit EventArgsMouse(MouseEvent mouseEvent, double value, const Vec2& mousePos);
        [[nodiscard]] std::string type() const override;
        [[nodiscard]] MouseEvent getMouseEvent() const;
        [[nodiscard]] double getValue() const;
        [[nodiscard]] const Vec2& getMousePos() const;
    private:
        MouseEvent mouseEvent;
        double value;
        Vec2 mousePos;
    };

    class Scene;
    enum class SceneOperation;

    class EventArgsScene : public EventArgs {
    public:
        EventArgsScene(SceneOperation op, Scene* scene);
        [[nodiscard]] std::string type() const override;
        [[nodiscard]] SceneOperation getOperation() const;
        [[nodiscard]] Scene* getScene() const;
    private:
        SceneOperation op;
        Scene* scene;
    };
}

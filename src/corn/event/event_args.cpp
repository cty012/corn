#include <corn/event/event_args.h>

namespace corn {
    std::string EventArgsExit::type() const {
        return "corn::input::exit";
    }

    EventArgsKeyboard::EventArgsKeyboard(Key key, KeyEvent keyEvent, unsigned char modifiers, const Vec2& mousePos)
        : key(key), keyEvent(keyEvent), modifiers(modifiers), mousePos(mousePos) {}

    std::string EventArgsKeyboard::type() const {
        return "corn::input::keyboard";
    }

    EventArgsMouse::EventArgsMouse(MouseEvent mouseEvent, double value, const Vec2& mousePos)
        : mouseEvent(mouseEvent), value(value), mousePos(mousePos) {}

    std::string EventArgsMouse::type() const {
        return "corn::input::mouse";
    }

    EventArgsScene::EventArgsScene(SceneOperation op, Scene *scene): scene(scene), op(op) {}

    std::string EventArgsScene::type() const {
        return "corn::game::scene";
    }
}

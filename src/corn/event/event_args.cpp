#include <corn/event/event_args.h>

namespace corn {
    std::string EventArgsClose::type() const {
        return "corn::input::close";
    }

    EventArgsKeyboard::EventArgsKeyboard(Key key, KeyEvent keyEvent, unsigned char modifiers, const Vec2& mousePos)
        : key(key), keyEvent(keyEvent), modifiers(modifiers), mousePos(mousePos) {}

    std::string EventArgsKeyboard::type() const {
        return "corn::input::keyboard";
    }

    Key EventArgsKeyboard::getKey() const {
        return this->key;
    }

    KeyEvent EventArgsKeyboard::getKeyEvent() const {
        return this->keyEvent;
    }

    unsigned char EventArgsKeyboard::getModifiers() const {
        return this->modifiers;
    }

    const Vec2& EventArgsKeyboard::getMousePos() const {
        return this->mousePos;
    }

    EventArgsMouse::EventArgsMouse(MouseEvent mouseEvent, double value, const Vec2& mousePos)
        : mouseEvent(mouseEvent), value(value), mousePos(mousePos) {}

    std::string EventArgsMouse::type() const {
        return "corn::input::mouse";
    }

    MouseEvent EventArgsMouse::getMouseEvent() const {
        return this->mouseEvent;
    }

    double EventArgsMouse::getValue() const {
        return this->value;
    }

    const Vec2& EventArgsMouse::getMousePos() const {
        return this->mousePos;
    }

    EventArgsScene::EventArgsScene(SceneOperation op, Scene *scene): scene(scene), op(op) {}

    std::string EventArgsScene::type() const {
        return "corn::game::scene";
    }

    SceneOperation EventArgsScene::getOperation() const {
        return this->op;
    }

    Scene* EventArgsScene::getScene() const {
        return this->scene;
    }
}

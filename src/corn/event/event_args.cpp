#include <corn/event.h>


namespace corn {
    EventArgsKeyboard::EventArgsKeyboard(Key key): key(key) {}

    std::string EventArgsKeyboard::type() const {
        return "corn::input::keyboard";
    }

    Key EventArgsKeyboard::getKey() const {
        return key;
    }

    EventArgsMouse::EventArgsMouse(Mouse mouse): mouse(mouse) {}

    std::string EventArgsMouse::type() const {
        return "corn::input::mouse";
    }

    Mouse EventArgsMouse::getMouse() const {
        return mouse;
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

#include <corn/event/event_args.h>

namespace corn {
    EventArgsKeyboard::EventArgsKeyboard(Key key, ButtonEvent status, unsigned char modifiers, const Vec2& mousePos)
        : key(key), status(status), modifiers(modifiers), mousePos(mousePos) {}

    EventArgsMouseButton::EventArgsMouseButton(Mouse mouse, ButtonEvent status, const Vec2& mousePos)
        : mouse(mouse), status(status), mousePos(mousePos) {}

    EventArgsMouseMove::EventArgsMouseMove(const Vec2 &mousePos) : mousePos(mousePos) {}

    EventArgsMouseScroll::EventArgsMouseScroll(double value, const Vec2 &mousePos)
        : value(value), mousePos(mousePos) {}

    EventArgsScene::EventArgsScene(SceneOperation op, Scene *scene): scene(scene), op(op) {}
}

#include <corn/event/event_args.h>

#include <utility>
#include "event_args_extend.h"

namespace corn {
    EventArgs::~EventArgs() = default;

    EventArgsKeyboard::EventArgsKeyboard(Key key, ButtonEvent status, unsigned char modifiers, const Vec2& mousePos)
            : key(key), status(status), modifiers(modifiers), mousePos(mousePos) {}

    EventArgsMouseButton::EventArgsMouseButton(Mouse mouse, ButtonEvent status, const Vec2& mousePos)
            : mouse(mouse), status(status), mousePos(mousePos) {}

    EventArgsMouseMove::EventArgsMouseMove(const Vec2& mousePos) : mousePos(mousePos) {}

    EventArgsMouseScroll::EventArgsMouseScroll(float value, const Vec2& mousePos)
            : value(value), mousePos(mousePos) {}

    EventArgsScene::EventArgsScene(SceneOperation op, Scene* scene): op(op), scene(scene) {}

    EventArgsZOrderChange::EventArgsZOrderChange(Entity* entity): entity(entity) {}

    EventArgsCamera::EventArgsCamera(CameraEventType eventType, const CCamera* camera)
            : eventType(eventType), camera(camera) {}

    EventArgsCollision::EventArgsCollision(CAABB* collider1, CAABB* collider2)
            : collider1(collider1), collider2(collider2) {}

    EventArgsUIOnClick::EventArgsUIOnClick(EventArgsMouseButton mousebtnEvent, UIWidget* target)
            : mousebtnEvent(std::move(mousebtnEvent)), target(target) {}

    EventArgsUIOnHover::EventArgsUIOnHover(EventArgsMouseMove mousemvEvent, UIWidget* target)
            : mousemvEvent(std::move(mousemvEvent)), target(target) {}

    EventArgsUIOnEnter::EventArgsUIOnEnter(EventArgsMouseMove mousemvEvent, UIWidget* target)
            : mousemvEvent(std::move(mousemvEvent)), target(target) {}

    EventArgsUIOnExit::EventArgsUIOnExit(EventArgsMouseMove mousemvEvent, UIWidget* target)
            : mousemvEvent(std::move(mousemvEvent)), target(target) {}
}

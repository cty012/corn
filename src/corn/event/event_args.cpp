#include <utility>
#include <corn/event/event_args.h>
#include "event_args_extend.h"

namespace corn {
    EventArgs::~EventArgs() = default;

    EventArgsKeyboard::EventArgsKeyboard(Key key, ButtonEvent status, unsigned char modifiers, const Vec2& mousePos) noexcept
            : key(key), status(status), modifiers(modifiers), mousePos(mousePos) {}

    EventArgsMouseButton::EventArgsMouseButton(Mouse mouse, ButtonEvent status, const Vec2& mousePos) noexcept
            : mouse(mouse), status(status), mousePos(mousePos) {}

    EventArgsMouseMove::EventArgsMouseMove(const Vec2& mousePos) noexcept : mousePos(mousePos) {}

    EventArgsMouseScroll::EventArgsMouseScroll(float value, const Vec2& mousePos) noexcept
            : value(value), mousePos(mousePos) {}

    EventArgsTextEntered::EventArgsTextEntered(unsigned int unicode, std::u8string character) noexcept
            : unicode(unicode), character(std::move(character)) {}

    EventArgsScene::EventArgsScene(SceneOperation op, Scene* scene) noexcept : op(op), scene(scene) {}

    EventArgsEntityZOrderChange::EventArgsEntityZOrderChange(Entity* entity) noexcept : entity(entity) {}

    EventArgsWidgetZOrderChange::EventArgsWidgetZOrderChange(UIWidget* widget) noexcept : widget(widget) {}

    EventArgsCamera::EventArgsCamera(CameraEventType eventType, const CCamera* camera) noexcept
            : eventType(eventType), camera(camera) {}

    EventArgsCollision::EventArgsCollision(CBBox* collider1, CBBox* collider2) noexcept
            : collider1(collider1), collider2(collider2) {}

    EventArgsUIOnClick::EventArgsUIOnClick(EventArgsMouseButton mousebtnEvent, UIWidget* target) noexcept
            : mousebtnEvent(std::move(mousebtnEvent)), target(target) {}

    EventArgsUIOnHover::EventArgsUIOnHover(EventArgsMouseMove mousemvEvent, UIWidget* target) noexcept
            : mousemvEvent(std::move(mousemvEvent)), target(target) {}

    EventArgsUIOnEnter::EventArgsUIOnEnter(EventArgsMouseMove mousemvEvent, UIWidget* target) noexcept
            : mousemvEvent(std::move(mousemvEvent)), target(target) {}

    EventArgsUIOnExit::EventArgsUIOnExit(EventArgsMouseMove mousemvEvent, UIWidget* target) noexcept
            : mousemvEvent(std::move(mousemvEvent)), target(target) {}

    EventArgsUIOnScroll::EventArgsUIOnScroll(EventArgsMouseScroll mousescEvent, UIWidget* target) noexcept
            : mousescEvent(std::move(mousescEvent)), target(target) {}
}

#include <utility>
#include <corn/event/event_args.h>
#include "event_args_extend.h"

namespace corn {
    EventArgs::~EventArgs() = default;

    EventArgsKeyboard::EventArgsKeyboard(Key key, ButtonEvent status, uint8_t modifiers, Vec2f mousePos) noexcept
            : key(key), status(status), modifiers(modifiers), mousePos(std::move(mousePos)) {}

    EventArgsWorldKeyboard::EventArgsWorldKeyboard(Key key, ButtonEvent status, uint8_t modifiers, Vec2f mousePos) noexcept
            : key(key), status(status), modifiers(modifiers), mousePos(std::move(mousePos)) {}

    EventArgsMouseButton::EventArgsMouseButton(MouseButton mouseButton, ButtonEvent status, Vec2f mousePos) noexcept
            : mouseButton(mouseButton), status(status), mousePos(std::move(mousePos)) {}

    EventArgsWorldMouseButton::EventArgsWorldMouseButton(MouseButton mouseButton, ButtonEvent status, Vec2f mousePos) noexcept
            : mouseButton(mouseButton), status(status), mousePos(std::move(mousePos)) {}

    EventArgsMouseMove::EventArgsMouseMove(Vec2f mousePos) noexcept : mousePos(std::move(mousePos)) {}

    EventArgsWorldMouseMove::EventArgsWorldMouseMove(Vec2f mousePos) noexcept : mousePos(std::move(mousePos)) {}

    EventArgsMouseScroll::EventArgsMouseScroll(Vec2f value, Vec2f mousePos) noexcept
            : value(std::move(value)), mousePos(std::move(mousePos)) {}

    EventArgsWorldMouseScroll::EventArgsWorldMouseScroll(Vec2f value, Vec2f mousePos) noexcept
            : value(std::move(value)), mousePos(std::move(mousePos)) {}

    EventArgsTextEntered::EventArgsTextEntered(std::string input) noexcept : input(std::move(input)) {}

    EventArgsScene::EventArgsScene(SceneOperation op, Scene* scene) noexcept : op(op), scene(scene) {}

    EventArgsEntityZOrderChange::EventArgsEntityZOrderChange(Entity* entity) noexcept : entity(entity) {}

    EventArgsWidgetZOrderChange::EventArgsWidgetZOrderChange(UIWidget* widget) noexcept : widget(widget) {}

    EventArgsCamera::EventArgsCamera(CameraEventType eventType, const CCamera* camera) noexcept
            : eventType(eventType), camera(camera) {}

    EventArgsCollision::EventArgsCollision(CBBox* collider1, CBBox* collider2) noexcept
            : collider1(collider1), collider2(collider2) {}

    EventArgsUIKeyboard::EventArgsUIKeyboard(EventArgsKeyboard keyboardEvent) noexcept
            : keyboardEvent(std::move(keyboardEvent)) {}

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

    EventArgsUIOnFocus::EventArgsUIOnFocus(UIWidget* target) noexcept : target(target) {}

    EventArgsUIOnUnfocus::EventArgsUIOnUnfocus(UIWidget* target) noexcept : target(target) {}
}

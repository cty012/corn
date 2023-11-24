#pragma once

#include <corn/event/event_args.h>

namespace corn {
    class Entity;

    struct EventArgsZOrderChange : public EventArgs {
        [[nodiscard]] constexpr const char* type() const override { return "corn::game::ecs::zorder"; }

        Entity* entity;

        explicit EventArgsZOrderChange(Entity* entity);
    };

    enum class CameraEventType { ADD, REMOVE };
    struct CCamera;

    struct EventArgsCamera : public EventArgs {
        [[nodiscard]] constexpr const char* type() const override { return "corn::game::ecs::camera"; }

        CameraEventType eventType;
        const CCamera* camera;

        EventArgsCamera(CameraEventType eventType, const CCamera* camera);
    };
}

#pragma once

#include <corn/ecs/entity.h>
#include <corn/event/event_args.h>

namespace corn {
    struct EventArgsZOrderChange : public EventArgs {
        [[nodiscard]] constexpr const char* type() const override { return "corn::game::ecs::zorder"; }
        explicit EventArgsZOrderChange(Entity* entity);
        Entity* entity;
    };

    enum class CameraEventType { ADD, REMOVE };
    struct EventArgsCamera : public EventArgs {
        [[nodiscard]] constexpr const char* type() const override { return "corn::game::ecs::camera"; }
        EventArgsCamera(CameraEventType eventType, const CCamera* camera);
        CameraEventType eventType;
        const CCamera* camera;
    };
}

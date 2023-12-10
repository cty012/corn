#pragma once

#include <corn/event/event_args.h>

namespace corn {
    class Entity;

    /**
     * @class EventArgsZOrderChange
     * @brief Emits when the z-order of an entity changes.
     */
    struct EventArgsZOrderChange : public EventArgs {
        [[nodiscard]] constexpr const char* type() const noexcept override { return "corn::game::ecs::zorder"; }

        /// @brief Entity whose z-order changes.
        Entity* entity;

        /// @brief Constructor.
        explicit EventArgsZOrderChange(Entity* entity) noexcept;
    };

    enum class CameraEventType { ADD, REMOVE };
    struct CCamera;

    /**
     * @class EventArgsCamera
     * @brief Emits when a camera is added to or removed from a scene.
     */
    struct EventArgsCamera : public EventArgs {
        [[nodiscard]] constexpr const char* type() const noexcept override { return "corn::game::ecs::camera"; }

        /// @brief Type of the camera event.
        CameraEventType eventType;

        /// @brief Target camera.
        const CCamera* camera;

        EventArgsCamera(CameraEventType eventType, const CCamera* camera) noexcept;
    };
}

#pragma once

#include <corn/event/event_args.h>

namespace corn {
    class Entity;

    /**
     * @class EventArgsEntityZOrderChange
     * @brief Emits when the z-order of an entity changes.
     */
    struct EventArgsEntityZOrderChange : public EventArgs {
        [[nodiscard]] constexpr std::string type() const noexcept override { return "corn::game::ecs::zorder"; }

        /// @brief Entity whose z-order changes.
        Entity* entity;

        /// @brief Constructor.
        explicit EventArgsEntityZOrderChange(Entity* entity) noexcept;
    };

    class UIWidget;

    /**
     * @class EventArgsEntityZOrderChange
     * @brief Emits when the z-order of an entity changes.
     */
    struct EventArgsWidgetZOrderChange : public EventArgs {
        [[nodiscard]] constexpr std::string type() const noexcept override { return "corn::game::ui::zorder"; }

        /// @brief Entity whose z-order changes.
        UIWidget* widget;

        /// @brief Constructor.
        explicit EventArgsWidgetZOrderChange(UIWidget* widget) noexcept;
    };

    enum class CameraEventType { ADD, REMOVE };
    struct CCamera;

    /**
     * @class EventArgsCamera
     * @brief Emits when a camera is added to or removed from a scene.
     */
    struct EventArgsCamera : public EventArgs {
        [[nodiscard]] constexpr std::string type() const noexcept override { return "corn::game::ecs::camera"; }

        /// @brief Type of the camera event.
        CameraEventType eventType;

        /// @brief Target camera.
        const CCamera* camera;

        EventArgsCamera(CameraEventType eventType, const CCamera* camera) noexcept;
    };
}

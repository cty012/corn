#pragma once

#include <utility>
#include <corn/geometry/deg.h>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/media/camera_viewport.h>
#include <corn/media/image.h>
#include <corn/util/expression.h>

namespace corn {
    class Entity;
    class EntityManager;

    /**
     * @class Component
     * @brief Base Component in the ECS architecture.
     *
     * In traditional ECS architecture, Components contain pure data. In the CORN engine, we do allow defining the
     * constructor, destructor, and other member functions for Components. However, it is highly recommended to move
     * complex logic to Systems for conciser code, if not for better compiler optimizations.
     *
     * @see Entity
     * @see System
     */
    struct Component {
        bool active;
        Entity& entity;
        EntityManager& entityManager;
        explicit Component(Entity& entity);
        virtual ~Component() = default;
    };

    /**
     * @class CTransform2D
     * @brief Stores the location and the rotation of the 2D object.
     *
     * @see Component
     * @see SMovement2D
     */
    struct CTransform2D : public Component {
        Vec2 location;
        Deg rotation;
        CTransform2D(Entity& entity, Vec2 location, Deg rotation = Deg());
        [[nodiscard]] std::pair<Vec2, Deg> worldTransform() const;
        void setWorldLocation(Vec2 newLocation);
        void addWorldLocationOffset(Vec2 offset);
        [[nodiscard]] int getZOrder() const;
        void setZOrder(int _zorder);
    private:
        int zorder;
    };

    /**
     * @class CSprite
     * @brief Stores the image of the Entity. A 2D Entity is only rendered if it has a CSprite Component and a
     * CTransform2D Component.
     *
     * @see Component
     * @see Image
     * @see Interface
     */
    struct CSprite : public Component {
        Image* image;
        Vec2 topLeft;
        bool visible;
        CSprite(Entity& entity, Image *image, Vec2 topLeft = Vec2::ZERO());
        ~CSprite() override;
    };

    /**
     * @class CMovement2D
     * @brief Stores the velocity of the object in 2D space.
     *
     * Unit: pixels/second & degrees/second
     *
     * @see Component
     * @see SMovement2D
     */
    struct CMovement2D : public Component {
        Vec2 velocity;
        float angularVelocity;
        explicit CMovement2D(Entity& entity, Vec2 velocity = Vec2::ZERO(), float angularVelocity = 0.0f);
        [[nodiscard]] std::pair<Vec2, float> worldMovement() const;
        void setWorldVelocity(Vec2 newVelocity);
        void addWorldVelocityOffset(Vec2 offset);
    };

    /**
     * @class CGravity2D
     * @brief Constantly increases world velocity.
     *
     * @see Component
     * @see SGravity
     */
    struct CGravity2D : public Component {
        float scale;
        explicit CGravity2D(Entity& entity, float scale = 1.0f);
    };

    /**
     * @class CAABB
     * @brief Axis-aligned bounding box (AABB), or a rectangular box for collision detection.
     *
     * The Vec2 for the corners are relative to the world location of the entity. i.e. <0, 0> would refer to the exact
     * location of the object. Having an invalid set of upper left and lower right corner will result in no collisions.
     *
     * Note that the AABB is not affected by rotation.
     *
     * @see Component
     * @see SCollisionDetection
     * @see CCollisionResolve
     */
    struct CAABB : public Component {
        /// @brief Upper left corner
        Vec2 ul;
        /// @brief Lower right corner
        Vec2 lr;
        CAABB(Entity& entity, Vec2 ul, Vec2 lr);

        /// @brief Check if two AABBs overlap
        [[nodiscard]] bool overlapWith(const CAABB& other) const;
    };

    /**
     * @class CCollisionResolve
     * @brief A simple collision resolve base class for collision between AABB colliding boxes.
     *
     * @todo Add support for all colliding boxes.
     *
     * @see Component
     * @see SCollisionDetection
     * @see CAABB
     */
    struct CCollisionResolve : public Component {
        explicit CCollisionResolve(Entity& entity);
        /// @brief Override this function to define custom collision resolves.
        virtual void resolve(CAABB& self, CAABB& other);
    };

    enum class CameraType{ _2D, _3D };

    /**
     * @class CCamera
     *
     * @todo Implement this
     */
    struct CCamera : public Component {
        /// @brief 2D camera renders entities with 2D transforms, and the same for 3D cameras.
        CameraType cameraType;

        /// @brief Whether the camera will be actively rendering.
        bool active;

        /// @brief The opacity of the camera, on a scale of [0, 255].
        unsigned char opacity;

        /**
         * @brief The location of the camera relative to the Transform.
         *
         * The rotation stored in the transform component is applied to the anchor, and then the result is added to the
         * location stored in the transform component. The result will be the center of the Camera.
         */
        Vec3 anchor;

        /// @brief The background color of the camera's field of view.
        Color background;

        /// @brief Viewport of the camera.
        CameraViewport viewport;

        /**
         * @brief Defines the field of view's width and height.
         *
         * The field of view is centered at the camera's location (stored in the Transform component).
         * Units include pixels (px), percentage of viewport width (%vw), and percentage of viewport height (%vh).
         *
         * If the FOV size doesn't match with the viewport size, the FOV will be stretched to fit the viewport.
         *
         * For detailed usage, see the documentation of @Expression.
         */
        Expression<3> fovW, fovH;

        /// @brief Creates a 2D camera
        CCamera(Entity& entity, Vec2 anchor, Color background = Color::rgb(0, 0, 0, 0));
        /// @brief Creates a 3D camera
        CCamera(Entity& entity, Vec3 anchor, Color background = Color::rgb(0, 0, 0, 0));

        ~CCamera() override;

        /**
         * @brief Set the top-left corner, width, and height of the viewport.
         *
         * E.g. To render to the entire window, use:
         * `setViewport("0px", "0px", "100%ww", "100%wh")`
         *
         * To maintain an aspect ratio of 16:9 while fitting inside the window, use:
         * ```
         * setViewport(
         *   "(100%ww - min(100%ww * 9, 100%wh * 16) / 9) / 2",
         *   "(100%wh - min(100%ww * 9, 100%wh * 16) / 16) / 2",
         *   "min(100%ww * 9, 100%wh * 16) / 9",
         *   "min(100%ww * 9, 100%wh * 16) / 16"
         * )
         * ```
         */
        void setViewport(const std::string& x, const std::string& y, const std::string& w, const std::string& h);

        /**
         * @brief Set the width and height of the field of view.
         *
         * Similar to @setViewport, but uses viewport width (%vw) and viewport height (%vh).
         *
         * To capture a fixed FOV (such as 1920x1080), use `setFov("1920px", "1080px")`.
         * To let the FOV change size with the viewport, use `setFov("100%w", "100%h")`.
         */
        void setFov(const std::string& w, const std::string& h);
    };
}

#pragma once

#include <corn/media/image.h>
#include <corn/util/geometry.h>

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
     * @brief Stores the location and the rotation of the object.
     *
     * @see Component
     * @see SMovement2D
     */
    struct CTransform2D : public Component {
        Vec2 location;
        Deg rotation;  // TODO: currently does not consider rotation
        CTransform2D(Entity& entity, Vec2 location, Deg rotation = Deg());
        [[nodiscard]] Vec2 worldLocation() const;
        [[nodiscard]] int getZOrder() const;
        void setZOrder(int _zorder);
    private:
        int zorder;
    };

    /**
     * @class CSprite
     * @brief Stores the image of the Entity. An Entity is only rendered if it has a CSprite Component.
     *
     * @see Component
     * @see Image
     * @see Interface
     */
    struct CSprite : public Component {
        Image* image;
        bool visible;
        CSprite(Entity& entity, Image *image);
        ~CSprite() override;
    };

    /**
     * @class CMovement2D
     * @brief Stores the velocity of the object in 2D space. Not affected by rotation.
     *
     * Unit: pixels/second
     *
     * @see Component
     * @see SMovement2D
     */
    struct CMovement2D : public Component {
        Vec2 velocity;
        explicit CMovement2D(Entity& entity, Vec2 velocity = {0, 0});
    };

    /**
     * @class CGravity2D
     * @brief Stores the velocity of the object. Not affected by rotation.
     *
     * @see Component
     * @see SGravity
     */
    struct CGravity2D : public Component {
        double scale;
        explicit CGravity2D(Entity& entity, double scale = 1.0);
    };

    /**
     * @class CAABB
     * @brief Axis-aligned bounding box (AABB), or a rectangular box for collision detection.
     *
     * The Vec2 for the corners are relative to the world location of the entity. i.e. <0, 0> would refer to the exact
     * location of the object. Having an invalid set of upper left and lower right corner will result in no collisions.
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
         * Suppose anchor = <x, y, z>, and transform is bounded by (<l, u>, <r, b>). The location of the camera is
         * defined as <l(1 - x) + rx, u(1 - y) + by>. Same for 3D case.
         */
        Vec3 anchor;

        // TODO: viewport

        CCamera(Entity& entity, CameraType cameraType, Vec3 anchor = Vec3::ZERO);
        ~CCamera();
    };
}

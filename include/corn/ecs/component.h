#pragma once

#include <corn/geometry/deg.h>
#include <corn/geometry/vec2.h>
#include <corn/media/image.h>

namespace corn {
    class Entity;

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
        [[nodiscard]] CTransform2D worldTransform() const;
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
        CSprite(Entity& entity, Image *image, Vec2 topLeft = Vec2::ZERO);
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
        explicit CMovement2D(Entity& entity, Vec2 velocity = Vec2::ZERO, float angularVelocity = 0.0f);
        [[nodiscard]] CMovement2D worldMovement() const;
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
}

#pragma once

#include <corn/media/image.h>
#include <corn/util/geometry.h>

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
     * @brief Stores the location and the rotation of the object.
     *
     * @see Component
     * @see SMovement2D
     */
    struct CTransform2D : public Component {
        Vec2 location;
        Deg rotation;  // TODO: currently does not consider rotation
        int zorder;
        CTransform2D(Entity& entity, Vec2 location, Deg rotation = Deg());
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
        CSprite(Entity& entity, Image *image);
        ~CSprite() override;
    };

    /**
     * @class CMovement2D
     * @brief Stores the velocity of the object in 2D space. Not affected by rotation.
     *
     * @see Component
     * @see SMovement2D
     */
    struct CMovement2D : public Component {
        Vec2 velocity;
        explicit CMovement2D(Entity& entity, Vec2 velocity = {0, 0});
    };

    /**
     * @class CGravity
     * @brief Stores the velocity of the object. Not affected by rotation.
     *
     * @see Component
     * @see SGravity
     */
    struct CGravity2D : public Component {
        double scale;
        CGravity2D(Entity& entity, double scale = 1.0);
    };

    /**
     * @class CAABB
     * @brief Axis-aligned bounding box (AABB), or a rectangular box for collision detection.
     *
     * The Vec2 for the corners are relative to the world location of the entity. i.e. <0, 0> would refer to the exact
     * location of the object. Having an invalid set of upper left and lower right corner will result in no collisions.
     */
    struct CAABB : public Component {
        /// @brief Upper left corner
        Vec2 ul;
        /// @brief Lower right corner
        Vec2 lr;
        CAABB(Entity& entity, Vec2 ul, Vec2 lr);
    };
}

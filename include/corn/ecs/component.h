#pragma once

#include <array>
#include <corn/ecs/entity.h>
#include <corn/geometry/deg.h>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/media/camera_viewport.h>
#include <corn/util/color.h>
#include <corn/util/expression.h>

namespace corn {
    class EntityManager;
    class Game;
    class Image;
    class Scene;

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
        /// @brief An inactive component will be ignored.
        bool active;

        /// @brief Constructor.
        explicit Component(Entity& entity) noexcept;

        /// @brief Destructor.
        virtual ~Component();

        /// @return The entity that owns this component.
        [[nodiscard]] Entity& getEntity() const noexcept;

        /// @return The entity manager that contains this component.
        [[nodiscard]] EntityManager& getEntityManager() const noexcept;

        /// @return The scene that contains this component.
        [[nodiscard]] Scene& getScene() const noexcept;

        /// @return The game that contains this component.
        [[nodiscard]] const Game* getGame() const noexcept;

    private:
        /// @brief The entity manager that contains the owner of this component.
        EntityManager& entityManager_;

        /// @brief The ID of the entity that owns this component.
        Entity::EntityID entityID_;
    };

    /**
     * @class CTransform2D
     * @brief Stores the location and the rotation of the 2D object.
     *
     * @see Component
     * @see SMovement2D
     */
    struct CTransform2D : public Component {
        /// @brief Location of the entity in its parent's reference frame.
        Vec2 location;

        /// @brief Rotation of the entity in its parent's reference frame.
        Deg rotation;

        /// @brief Constructor.
        CTransform2D(Entity& entity, Vec2 location, Deg rotation = Deg()) noexcept;

        /// @return The transform in the world's reference frame.
        [[nodiscard]] std::pair<Vec2, Deg> getWorldTransform() const noexcept;

        /// @brief Set the location of the entity in the world's reference frame.
        void setWorldLocation(Vec2 worldLocation) noexcept;

        /// @brief Adds an offset to the location of the entity in the world's reference frame.
        void addWorldLocationOffset(Vec2 offset) noexcept;

        /// @brief Getter of the z-order of the entity.
        [[nodiscard]] int getZOrder() const noexcept;

        /// @brief Setter of the z-order of the entity.
        void setZOrder(int zOrder) noexcept;

    private:
        /**
         * @brief Defines the order of the entities in the z direction (in/out of the screen). A higher z-order means
         * closer to the top.
         */
        int zOrder_;
    };

    /**
     * @class CBBox
     * @brief Axis-aligned bounding box (BBox), or a rectangular box for collision detection.
     *
     * The Vec2 for the corners are relative to the world location of the entity. i.e. <0, 0> would refer to the exact
     * location of the object. Having an invalid set of top left and bottom right corner will result in no collisions.
     *
     * Note that the BBox is not affected by rotation.
     *
     * @see Component
     * @see SCollisionDetection
     * @see CCollisionResolve
     */
    struct CBBox : public Component {
        /// @brief Location of the top left corner.
        Vec2 tl;

        /// @brief Location of the bottom right corner.
        Vec2 br;

        /// @brief Constructor.
        CBBox(Entity& entity, Vec2 tl, Vec2 br) noexcept;

        /// @return Whether the two AABBs overlap.
        [[nodiscard]] bool overlapWith(const CBBox& other) const noexcept;
    };

    struct CLines : public Component {
        /// @brief A vector of vertices.
        std::vector<Vec2> vertices;

        /// @brief Thickness of the edges. If thickness is less than or equal to 0 then fill the polygon.
        float thickness;

        /// @brief Color of the edges.
        Color color;

        /// @brief Whether there is a connection between the last point and the first point.
        bool closed;

        /// @brief Constructor.
        CLines(Entity& entity, std::vector<Vec2> vertices, float thickness, const Color& color, bool closed = false) noexcept;
    };

    struct CPolygon : public Component {
        /// @brief Thickness of the edges. If thickness is less than or equal to 0 then fill the polygon.
        float thickness;

        /// @brief Color of the edges.
        Color color;

        /// @brief Constructor.
        CPolygon(Entity& entity, std::vector<std::vector<Vec2>> vertices, float thickness, const Color& color) noexcept;

        /// @brief Getter of the vertices
        [[nodiscard]] const std::vector<std::vector<Vec2>>& getVertices() const noexcept;

        /// @brief Setter of the vertices
        void setVertices(std::vector<std::vector<Vec2>> vertices);

        /// @return List of triangles as the result of triangulation.
        [[nodiscard]] const std::vector<std::array<Vec2, 3>>& getTriangles() const noexcept;

        [[nodiscard]] const std::pair<Vec2, Vec2>& getBBox() const noexcept;

        /**
         * @param point The target point.
         * @param countEdges Whether lying on the boundary qualifies as contained.
         * @return Whether the point is contained inside the polygon.
         */
        [[nodiscard]] bool contains(const Vec2& point, bool countEdges) const noexcept;

        [[nodiscard]] float getArea() const noexcept;

        [[nodiscard]] Vec2 getCentroid() const noexcept;

    private:
        /// @brief First element is the boundary. Subsequent elements are holes.
        std::vector<std::vector<Vec2>> vertices_;

        /// @brief The axis-aligned bounding box.
        std::pair<Vec2, Vec2> bBox;

        /// @brief A list of triangles (result of triangulation).
        std::vector<std::array<Vec2, 3>> triangles_;

        /// @brief Area of the polygon.
        float area_;

        /// @brief Centroid of the polygon.
        Vec2 centroid_;
    };

    /**
     * @class CSprite
     * @brief Stores the image of the entity. A 2D entity is only rendered if it has a CSprite Component and a
     * CTransform2D Component.
     *
     * @see Component
     * @see Image
     * @see Interface
     */
    struct CSprite : public Component {
        /// @brief Pointer to the image.
        Image* image;

        /// @brief Location of the top left corner of the image.
        Vec2 location;

        /// @brief Constructor.
        CSprite(Entity& entity, Image* image, Vec2 location = Vec2::ZERO()) noexcept;

        /// @brief Destructor.
        ~CSprite() override;
    };

    /**
     * @class CMovement2D
     * @brief Stores the velocity of the object in 2D space.
     *
     * @see Component
     * @see SMovement2D
     */
    struct CMovement2D : public Component {
        /**
         * @brief Linear velocity of the entity in its parent's reference frame.
         *
         * Unit: pixel/second
         */
        Vec2 velocity;

        /**
         * @brief Angular velocity of the entity in its parent's reference frame.
         *
         * Unit: degree/second
         */
        float angularVelocity;

        /// @brief Constructor.
        explicit CMovement2D(Entity& entity, Vec2 velocity = Vec2::ZERO(), float angularVelocity = 0.0f) noexcept;

        /// @return The velocities in the world's reference frame.
        [[nodiscard]] std::pair<Vec2, float> getWorldMovement() const noexcept;

        /// @brief Set the linear velocity of the entity in the world's reference frame.
        void setWorldVelocity(Vec2 worldVelocity) noexcept;

        /// @brief Adds an offset to the linear velocity of the entity in the world's reference frame.
        void addWorldVelocityOffset(Vec2 offset) noexcept;
    };

    /**
     * @class CGravity2D
     * @brief Constantly increases world velocity in the positive y direction.
     *
     * @see Component
     * @see SGravity
     */
    struct CGravity2D : public Component {
        /// @brief Scale of the gravitational acceleration on the entity.
        float scale;

        /// @brief Constructor.
        explicit CGravity2D(Entity& entity, float scale = 1.0f) noexcept;
    };

    enum class CameraType { _2D, _3D };

    /**
     * @class CCamera
     * @brief Captures graphics from the world and renders to the window.
     *
     * @see Component
     * @see Interface
     */
    struct CCamera : public Component {
        /// @brief 2D camera renders entities with 2D transforms, and the same for 3D cameras.
        CameraType cameraType;

        /// @brief The background color of the camera's field of view.
        Color background;

        /// @brief The opacity of the camera, on a scale of [0, 255].
        unsigned char opacity;

        /**
         * @brief The location of the camera relative to the Transform.
         *
         * The rotation stored in the transform component is applied to the anchor, and then the result is added to the
         * location stored in the transform component. The result will be the center of the Camera.
         */
        Vec3 anchor;

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

        /// @brief Scale of the camera's field of view. Applied after evaluating the expressions.
        float scale;

        /// @brief Constructor for 2D camera.
        CCamera(Entity& entity, Vec2 anchor, Color background = Color::rgb(0, 0, 0, 0)) noexcept;

        /// @brief Constructor for 3D camera.
        CCamera(Entity& entity, Vec3 anchor, Color background = Color::rgb(0, 0, 0, 0)) noexcept;

        /// @brief Destructor.
        ~CCamera() override;

        /**
         * @brief Set the top-left corner, width, and height of the viewport.
         *
         * Each parameter is an expression of px, %ww (percentage window width), and %wh (percentage window height). If
         * a parameter is an emtpy string, then its value will not be reassigned.
         *
         * @example
         * To render to the entire window, use:
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
         * Similar to setViewport, but uses viewport width (%vw) and viewport height (%vh). If a parameter is an emtpy
         * string, then its value will not be reassigned.
         *
         * @example
         * To capture a fixed FOV (such as 1920x1080), use `setFov("1920px", "1080px")`.
         * To let the FOV resize with the viewport, use `setFov("100%vw", "100%vh")`.
         */
        void setFov(const std::string& w, const std::string& h);
    };
}

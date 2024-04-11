#include <mapbox/earcut.hpp>
#include <corn/core/scene.h>
#include <corn/ecs/component.h>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>
#include <corn/geometry/operations.h>
#include <corn/media/image.h>
#include "../event/event_args_extend.h"

namespace corn {
    Component::Component(Entity& entity) noexcept : active(true), entity(entity) {}

    Component::~Component() = default;

    Entity& Component::getEntity() const noexcept {
        return this->entity;
    }

    EntityManager& Component::getEntityManager() const noexcept {
        return this->entity.getEntityManager();
    }

    Scene& Component::getScene() const noexcept {
        return this->entity.getScene();
    }

    const Game* Component::getGame() const noexcept {
        return this->entity.getGame();
    }

    CTransform2D::CTransform2D(Entity &entity, Vec2 location, Deg rotation) noexcept
            : Component(entity), location(location), rotation(rotation), zOrder_(0) {}

    std::pair<Vec2, Deg> CTransform2D::getWorldTransform() const noexcept {
        std::pair<Vec2, Deg> worldTransform = { this->location, this->rotation };
        for (Entity* ancestor = this->getEntity().getParent(); ancestor; ancestor = ancestor->getParent()) {
            auto* ancestorTransform = ancestor->getComponent<CTransform2D>();
            if (ancestorTransform) {
                worldTransform.second += ancestorTransform->rotation;
                worldTransform.first = rotate(worldTransform.first, ancestorTransform->rotation);
                worldTransform.first += ancestorTransform->location;
            }
        }
        return worldTransform;
    }

    void CTransform2D::setWorldLocation(Vec2 worldLocation) noexcept {
        auto [curWorldLocation, curWorldRotation] = this->getWorldTransform();
        Deg parentWorldRotation = curWorldRotation - this->rotation;
        Vec2 parentWorldLocation = curWorldLocation - rotate(this->location, parentWorldRotation);
        this->location = rotate(worldLocation - parentWorldLocation, -parentWorldRotation);
    }

    void CTransform2D::addWorldLocationOffset(Vec2 offset) noexcept {
        Deg curWorldRotation = this->getWorldTransform().second;
        Deg parentWorldRotation = curWorldRotation - this->rotation;
        this->location += rotate(offset, -parentWorldRotation);
    }

    int CTransform2D::getZOrder() const noexcept {
        return this->zOrder_;
    }

    void CTransform2D::setZOrder(int zOrder) noexcept {
        this->zOrder_ = zOrder;
        this->getScene().getEventManager().emit(EventArgsEntityZOrderChange(&this->getEntity()));
    }

    CBBox::CBBox(Entity& entity, Vec2 tl, Vec2 br) noexcept : Component(entity), tl(tl), br(br) {}

    bool CBBox::overlapWith(const CBBox& other) const noexcept {
        auto* transform1 = this->getEntity().getComponent<CTransform2D>();
        auto* transform2 = other.getEntity().getComponent<CTransform2D>();
        if (!transform1 || !transform2) return false;
        Vec2 worldLocation1 = transform1->getWorldTransform().first;
        Vec2 worldLocation2 = transform2->getWorldTransform().first;
        Vec2 tl1 = this->tl + worldLocation1;
        Vec2 br1 = this->br + worldLocation1;
        Vec2 tl2 = other.tl + worldLocation2;
        Vec2 br2 = other.br + worldLocation2;
        bool xDirection = std::min(br1.x, br2.x) > std::max(tl1.x, tl2.x);
        bool yDirection = std::min(br1.y, br2.y) > std::max(tl1.y, tl2.y);
        return xDirection && yDirection;
    }

    CLines::CLines(Entity& entity, std::vector<Vec2> vertices, float thickness, const Color& color, bool closed) noexcept
            : Component(entity), vertices(std::move(vertices)), thickness(thickness), color(color), closed(closed) {}

    CPolygon::CPolygon(Entity& entity, std::vector<std::vector<Vec2>> vertices, float thickness, const Color& color) noexcept
            : Component(entity), thickness(thickness), color(color) {

        this->setVertices(std::move(vertices));
    }

    const std::vector<std::vector<Vec2>>& CPolygon::getVertices() const noexcept {
        return this->vertices_;
    }

    void CPolygon::setVertices(std::vector<std::vector<Vec2>> vertices) {
        this->vertices_ = std::move(vertices);

        // Flatten the vertices
        std::vector<Vec2> flattened;
        size_t totalSize = 0;
        for (const std::vector<Vec2>& ring : this->vertices_) {
            totalSize += ring.size();
        }
        flattened.reserve(totalSize);
        for (const std::vector<Vec2>& ring : this->vertices_) {
            flattened.insert(flattened.end(), ring.begin(), ring.end());
        }

        // Find BBox
        this->bBox.first = this->bBox.second = flattened.empty() ? Vec2::ZERO() : flattened[0];
        for (const Vec2& point : flattened) {
            this->bBox.first.x = std::min(this->bBox.first.x, point.x);
            this->bBox.first.y = std::min(this->bBox.first.y, point.y);
            this->bBox.second.x = std::max(this->bBox.second.x, point.x);
            this->bBox.second.y = std::max(this->bBox.second.y, point.y);
        }

        // Triangulation
        using Point = std::array<float, 2>;
        std::vector<std::vector<Point>> earcutInput;
        for (const std::vector<Vec2>& ring : this->vertices_) {
            std::vector<Point>& back = earcutInput.emplace_back();
            back.reserve(ring.size());
            for (const Vec2& coord : ring) {
                back.push_back({ coord.x, coord.y });
            }
        }

        std::vector<size_t> indices = mapbox::earcut<size_t>(earcutInput);

        // Unflatten and store the triangle indices
        this->triangles_.clear();
        for (size_t i = 0; i * 3 + 2 < indices.size(); i++) {
            this->triangles_.push_back({ flattened[indices[i * 3 + 0]], flattened[indices[i * 3 + 1]], flattened[indices[i * 3 + 2]] });
        }
    }

    const std::vector<std::array<Vec2, 3>>& CPolygon::getTriangles() const noexcept {
        return this->triangles_;
    }

    const std::pair<Vec2, Vec2>& CPolygon::getBBox() const noexcept {
        return this->bBox;
    }

    bool CPolygon::contains(const Vec2& point, bool countEdges) const noexcept {
        if (this->vertices_.empty()) {
            return false;
        }

        // First check if it is in the bounding box
        const auto& [tl, br] = this->bBox;
        if (tl.x > point.x || tl.y > point.y || br.x < point.x || br.y < point.y) {
            return false;
        }

        // If so, first check if it is on the boundary
        for (size_t i = 0; i < this->vertices_.size(); i++) {
            if (this->vertices_[i].size() < 2) continue;
            for (size_t j = 0; j < this->vertices_[i].size(); j++) {
                auto [x1, y1] = this->vertices_[i][j];
                auto [x2, y2] = this->vertices_[i][(j + 1) % this->vertices_.size()];
                // Linearity check
                bool isCollinear = x1 * (y2 - point.y) + x2 * (point.y - y1) + point.x * (y1 - y2) == 0;
                // Between check
                bool xIsBetween = (std::min(x1, x2) <= point.x) && (point.x <= std::max(x1, x2));
                bool yIsBetween = (std::min(y1, y2) <= point.y) && (point.y <= std::max(y1, y2));
                if (isCollinear && xIsBetween && yIsBetween) {
                    return countEdges;
                }
            }
        }

        // Then check if it is inside/on the polygon
        return std::any_of(
                this->triangles_.begin(), this->triangles_.end(),
                [&point](const std::array<Vec2, 3>& triangle) {
            const auto& [p1, p2, p3] = triangle;
            auto [x1, y1] = p1;
            auto [x2, y2] = p2;
            auto [x3, y3] = p3;
            float cross1 = (x2 - x1) * (point.y - y1) - (point.x - x1) * (y2 - y1);
            float cross2 = (x3 - x2) * (point.y - y2) - (point.x - x2) * (y3 - y2);
            float cross3 = (x1 - x3) * (point.y - y3) - (point.x - x3) * (y1 - y3);
            return (cross1 >= 0 && cross2 >= 0 && cross3 >= 0) || (cross1 <= 0 && cross2 <= 0 && cross3 <= 0);
        });
    }

    CSprite::CSprite(Entity& entity, Image *image, Vec2 location) noexcept
            : Component(entity), image(image), location(location) {}

    CSprite::~CSprite() {
        delete this->image;
    }

    CMovement2D::CMovement2D(Entity& entity, Vec2 velocity, float angularVelocity) noexcept
            : Component(entity), velocity(velocity), angularVelocity(angularVelocity) {}

    std::pair<Vec2, float> CMovement2D::getWorldMovement() const noexcept {
        std::pair<Vec2, float> movement = { this->velocity, this->angularVelocity };
        for (Entity* ancestor = this->getEntity().getParent(); ancestor; ancestor = ancestor->getParent()) {
            auto* ancestorMovement = ancestor->getComponent<CMovement2D>();
            if (ancestorMovement) {
                movement.second += ancestorMovement->angularVelocity;
                auto* ancestorTransform = ancestor->getComponent<CTransform2D>();
                if (ancestorTransform) {
                    movement.first = rotate(movement.first, ancestorTransform->rotation);
                    movement.first += ancestorMovement->velocity;
                }
            }
        }
        return movement;
    }

    void CMovement2D::setWorldVelocity(Vec2 worldVelocity) noexcept {
        // Calculate parent rotation
        Deg parentWorldRotation = 0.0f;
        for (Entity* ancestor = this->getEntity().getParent(); ancestor; ancestor = ancestor->getParent()) {
            auto ancestorTransform = ancestor->getComponent<CTransform2D>();
            if (ancestorTransform) {
                parentWorldRotation += ancestorTransform->rotation;
            }
        }

        // Calculate parent world velocity
        Vec2 curWorldVelocity = this->getWorldMovement().first;
        Vec2 parentWorldVelocity = curWorldVelocity - rotate(this->velocity, parentWorldRotation);

        // Set current velocity
        this->velocity = rotate(worldVelocity - parentWorldVelocity, -parentWorldRotation);
    }

    void CMovement2D::addWorldVelocityOffset(Vec2 offset) noexcept {
        // Calculate parent rotation
        Deg parentWorldRotation = 0.0f;
        for (Entity* ancestor = this->getEntity().getParent(); ancestor; ancestor = ancestor->getParent()) {
            auto ancestorTransform = ancestor->getComponent<CTransform2D>();
            if (ancestorTransform) {
                parentWorldRotation += ancestorTransform->rotation;
            }
        }

        // Add offset to velocity
        this->velocity += rotate(offset, -parentWorldRotation);
    }

    CGravity2D::CGravity2D(Entity& entity, float scale) noexcept : Component(entity), scale(scale) {}

    CCamera::CCamera(Entity& entity, Vec2 anchor, Color background) noexcept
            : Component(entity), cameraType(CameraType::_2D), background(background), opacity(255),
            anchor(anchor.vec3(0)), scale(1.0f) {

        this->setViewport("0px", "0px", "100%ww", "100%wh");
        this->setFov("100%vw", "100%vh");
        this->getScene().getEventManager().emit(EventArgsCamera(CameraEventType::ADD, this));
    }

    CCamera::CCamera(Entity& entity, Vec3 anchor, Color background) noexcept
            : Component(entity), cameraType(CameraType::_3D), background(background), opacity(255),
            anchor(anchor), scale(1.0f) {

        this->getScene().getEventManager().emit(EventArgsCamera(CameraEventType::ADD, this));
    }

    CCamera::~CCamera() {
        this->getScene().getEventManager().emit(EventArgsCamera(CameraEventType::REMOVE, this));
    }

    void CCamera::setViewport(const std::string& x, const std::string& y, const std::string& w, const std::string& h) {
        static const std::array<std::string, 3> units = { "px", "%ww", "%wh" };
        if (!x.empty()) {
            this->viewport.x = Expression(x, units);
        }
        if (!y.empty()) {
            this->viewport.y = Expression(y, units);
        }
        if (!w.empty()) {
            this->viewport.w = Expression(w, units);
        }
        if (!h.empty()) {
            this->viewport.h = Expression(h, units);
        }
    }

    void CCamera::setFov(const std::string& w, const std::string& h) {
        static const std::array<std::string, 3> units = { "px", "%vw", "%vh" };
        if (!w.empty()) {
            this->fovW = Expression(w, units);
        }
        if (!h.empty()) {
            this->fovH = Expression(h, units);
        }
    }
}

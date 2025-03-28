#include <mapbox/earcut.hpp>
#include <utility>
#include <corn/core/scene.h>
#include <corn/ecs/component.h>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>
#include <corn/geometry/operations.h>
#include <corn/media/image.h>
#include "../event/event_args_extend.h"

namespace corn {
    Component::Component(Entity& entity) noexcept
            : active(true), entityManager_(entity.getEntityManager()), entityID_(entity.getID()) {}

    Component::~Component() = default;

    Entity& Component::getEntity() const noexcept {
        return *this->entityManager_.getEntityByID(this->entityID_);
    }

    EntityManager& Component::getEntityManager() const noexcept {
        return this->entityManager_;
    }

    Scene& Component::getScene() const noexcept {
        return this->entityManager_.getScene();
    }

    const Game* Component::getGame() const noexcept {
        return this->entityManager_.getGame();
    }

    CTransform2D::CTransform2D(Entity &entity, Vec2f location, Deg rotation) noexcept
            : Component(entity), location(location), rotation(rotation), zOrder_(0) {}

    std::pair<Vec2f, Deg> CTransform2D::getWorldTransform() const noexcept {
        std::pair<Vec2f, Deg> worldTransform = { this->location, this->rotation };
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

    void CTransform2D::setWorldLocation(Vec2f worldLocation) noexcept {
        auto [curWorldLocation, curWorldRotation] = this->getWorldTransform();
        Deg parentWorldRotation = curWorldRotation - this->rotation;
        Vec2f parentWorldLocation = curWorldLocation - rotate(this->location, parentWorldRotation);
        this->location = rotate(worldLocation - parentWorldLocation, -parentWorldRotation);
    }

    void CTransform2D::addWorldLocationOffset(Vec2f offset) noexcept {
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

    CBBox::CBBox(Entity& entity, Vec2f tl, Vec2f br) noexcept : Component(entity), tl(tl), br(br) {}

    bool CBBox::overlapWith(const CBBox& other) const noexcept {
        auto* transform1 = this->getEntity().getComponent<CTransform2D>();
        auto* transform2 = other.getEntity().getComponent<CTransform2D>();
        if (!transform1 || !transform2) return false;
        Vec2f worldLocation1 = transform1->getWorldTransform().first;
        Vec2f worldLocation2 = transform2->getWorldTransform().first;
        Vec2f tl1 = this->tl + worldLocation1;
        Vec2f br1 = this->br + worldLocation1;
        Vec2f tl2 = other.tl + worldLocation2;
        Vec2f br2 = other.br + worldLocation2;
        bool xDirection = std::min(br1.x, br2.x) > std::max(tl1.x, tl2.x);
        bool yDirection = std::min(br1.y, br2.y) > std::max(tl1.y, tl2.y);
        return xDirection && yDirection;
    }

    CLines::CLines(Entity& entity, std::vector<Vec2f> vertices, float thickness, const Color& color, bool closed) noexcept
            : Component(entity), vertices(std::move(vertices)), thickness(thickness), color(color), closed(closed) {}

    CPolygon::CPolygon(Entity& entity, Polygon polygon, float thickness, const Color& color) noexcept
            : Component(entity), polygon(std::move(polygon)), thickness(thickness), color(color) {}

    CSprite::CSprite(Entity& entity, Image *image, Vec2f location) noexcept
            : Component(entity), image(image), location(location) {}

    CSprite::~CSprite() {
        delete this->image;
    }

    CText::CText(Entity& entity, const TextRender& textRender) noexcept : Component(entity), textRender(textRender) {
        this->setX("0px");
        this->setY("0px");
    }

    CText::CText(Entity& entity, const TextRender& textRender, const std::string& x, const std::string& y) noexcept
            : Component(entity), textRender(textRender) {

        this->setX(x);
        this->setY(y);
    }

    float CText::getX() const noexcept {
        Vec2f percSize = this->textRender.getSize() * 0.01f;
        return this->x_.calc(1.0f, percSize.x, percSize.y);
    }

    void CText::setX(const std::string& x) noexcept {
        static const std::array<std::string, 3> units = { "px", "%w", "%h" };
        this->x_ = Expression(x, units);
    }

    float CText::getY() const noexcept {
        Vec2f percSize = this->textRender.getSize() * 0.01f;
        return this->y_.calc(1.0f, percSize.x, percSize.y);
    }

    void CText::setY(const std::string& y) noexcept {
        static const std::array<std::string, 3> units = { "px", "%w", "%h" };
        this->y_ = Expression(y, units);
    }

    CMovement2D::CMovement2D(Entity& entity, Vec2f velocity, float angularVelocity) noexcept
            : Component(entity), velocity(velocity), angularVelocity(angularVelocity) {}

    std::pair<Vec2f, float> CMovement2D::getWorldMovement() const noexcept {
        std::pair<Vec2f, float> movement = { this->velocity, this->angularVelocity };
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

    void CMovement2D::setWorldVelocity(Vec2f worldVelocity) noexcept {
        // Calculate parent rotation
        Deg parentWorldRotation = 0.0f;
        for (Entity* ancestor = this->getEntity().getParent(); ancestor; ancestor = ancestor->getParent()) {
            auto ancestorTransform = ancestor->getComponent<CTransform2D>();
            if (ancestorTransform) {
                parentWorldRotation += ancestorTransform->rotation;
            }
        }

        // Calculate parent world velocity
        Vec2f curWorldVelocity = this->getWorldMovement().first;
        Vec2f parentWorldVelocity = curWorldVelocity - rotate(this->velocity, parentWorldRotation);

        // Set current velocity
        this->velocity = rotate(worldVelocity - parentWorldVelocity, -parentWorldRotation);
    }

    void CMovement2D::addWorldVelocityOffset(Vec2f offset) noexcept {
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

    CCamera::CCamera(Entity& entity, Vec2f anchor, Color background) noexcept
            : Component(entity), cameraType(CameraType::_2D), background(background), opacity(255),
            anchor(anchor.to<3>()), scale(1.0f) {

        this->setViewport("0px", "0px", "100%ww", "100%wh");
        this->setFov("100%vw", "100%vh");
        this->getScene().getEventManager().emit(EventArgsCamera(CameraEventType::ADD, this));
    }

    CCamera::CCamera(Entity& entity, Vec3f anchor, Color background) noexcept
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

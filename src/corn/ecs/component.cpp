#include <array>
#include <corn/core/scene.h>
#include <corn/ecs/component.h>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>
#include <corn/event/event_manager.h>
#include "../event/event_args_extend.h"

namespace corn {
    Component::Component(Entity& entity) : active(true), entity(entity) {}

    Component::~Component() = default;

    Entity& Component::getEntity() const {
        return this->entity;
    }

    EntityManager& Component::getEntityManager() const {
        return this->entity.getEntityManager();
    }

    Scene& Component::getScene() const {
        return this->entity.getScene();
    }

    const Game* Component::getGame() const {
        return this->entity.getGame();
    }

    CTransform2D::CTransform2D(Entity &entity, Vec2 location, Deg rotation)
            : Component(entity), location(location), rotation(rotation), zOrder_(0) {}

    std::pair<Vec2, Deg> CTransform2D::getWorldTransform() const {
        std::pair<Vec2, Deg> worldTransform = {this->location, this->rotation};
        for (Entity* ancestor = this->getEntity().getParent(); ancestor; ancestor = ancestor->getParent()) {
            auto* ancestorTransform = ancestor->getComponent<CTransform2D>();
            if (ancestorTransform) {
                worldTransform.second += ancestorTransform->rotation;
                worldTransform.first = ancestorTransform->rotation.rotate(worldTransform.first);
                worldTransform.first += ancestorTransform->location;
            }
        }
        return worldTransform;
    }

    void CTransform2D::setWorldLocation(Vec2 worldLocation) {
        auto [curWorldLocation, curWorldRotation] = this->getWorldTransform();
        Deg parentWorldRotation = curWorldRotation - this->rotation;
        Vec2 parentWorldLocation = curWorldLocation - parentWorldRotation.rotate(this->location);
        this->location = (-parentWorldRotation).rotate(worldLocation - parentWorldLocation);
    }

    void CTransform2D::addWorldLocationOffset(Vec2 offset) {
        Deg curWorldRotation = this->getWorldTransform().second;
        Deg parentWorldRotation = curWorldRotation - this->rotation;
        this->location += (-parentWorldRotation).rotate(offset);
    }

    int CTransform2D::getZOrder() const {
        return this->zOrder_;
    }

    void CTransform2D::setZOrder(int zOrder) {
        this->zOrder_ = zOrder;
        this->getScene().getEventManager().emit(EventArgsZOrderChange(&this->getEntity()));
    }

    CSprite::CSprite(Entity& entity, Image *image, Vec2 location)
            : Component(entity), image(image), location(location) {}

    CSprite::~CSprite() {
        delete this->image;
    }

    CMovement2D::CMovement2D(Entity& entity, Vec2 velocity, float angularVelocity)
            : Component(entity), velocity(velocity), angularVelocity(angularVelocity) {}

    std::pair<Vec2, float> CMovement2D::getWorldMovement() const {
        std::pair<Vec2, float> movement = {this->velocity, this->angularVelocity};
        for (Entity* ancestor = this->getEntity().getParent(); ancestor; ancestor = ancestor->getParent()) {
            auto* ancestorMovement = ancestor->getComponent<CMovement2D>();
            if (ancestorMovement) {
                movement.second += ancestorMovement->angularVelocity;
                auto* ancestorTransform = ancestor->getComponent<CTransform2D>();
                if (ancestorTransform) {
                    movement.first = ancestorTransform->rotation.rotate(movement.first);
                    movement.first += ancestorMovement->velocity;
                }
            }
        }
        return movement;
    }

    void CMovement2D::setWorldVelocity(Vec2 worldVelocity) {
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
        Vec2 parentWorldVelocity = curWorldVelocity - parentWorldRotation.rotate(this->velocity);

        // Set current velocity
        this->velocity = (-parentWorldRotation).rotate(worldVelocity - parentWorldVelocity);
    }

    void CMovement2D::addWorldVelocityOffset(Vec2 offset) {
        // Calculate parent rotation
        Deg parentWorldRotation = 0.0f;
        for (Entity* ancestor = this->getEntity().getParent(); ancestor; ancestor = ancestor->getParent()) {
            auto ancestorTransform = ancestor->getComponent<CTransform2D>();
            if (ancestorTransform) {
                parentWorldRotation += ancestorTransform->rotation;
            }
        }

        // Add offset to velocity
        this->velocity += (-parentWorldRotation).rotate(offset);
    }

    CGravity2D::CGravity2D(Entity& entity, float scale) : Component(entity), scale(scale) {}

    CAABB::CAABB(Entity& entity, Vec2 tl, Vec2 br) : Component(entity), tl(tl), br(br) {}

    bool CAABB::overlapWith(const CAABB& other) const {
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

    CCamera::CCamera(Entity& entity, Vec2 anchor, Color background)
            : Component(entity), cameraType(CameraType::_2D), background(background), opacity(255),
            anchor(anchor.vec3(0)) {

        this->setViewport("0px", "0px", "100%ww", "100%wh");
        this->setFov("100%vw", "100%vh");
        this->getScene().getEventManager().emit(EventArgsCamera(CameraEventType::ADD, this));
    }

    CCamera::CCamera(Entity& entity, Vec3 anchor, Color background)
            : Component(entity), cameraType(CameraType::_3D), background(background), opacity(255), anchor(anchor) {

        this->getScene().getEventManager().emit(EventArgsCamera(CameraEventType::ADD, this));
    }

    CCamera::~CCamera() {
        this->getScene().getEventManager().emit(EventArgsCamera(CameraEventType::REMOVE, this));
    }

    void CCamera::setViewport(const std::string& x, const std::string& y, const std::string& w, const std::string& h) {
        static const std::array<std::string, 3> units = {"px", "%ww", "%wh"};
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
        static const std::array<std::string, 3> units = {"px", "%vw", "%vh"};
        if (!w.empty()) {
            this->fovW = Expression(w, units);
        }
        if (!h.empty()) {
            this->fovH = Expression(h, units);
        }
    }
}

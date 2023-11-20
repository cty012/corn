#include <cmath>
#include <array>
#include <corn/core/scene.h>
#include <corn/ecs/component.h>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>
#include <corn/event/event_manager.h>
#include "../event/event_args_extend.h"

namespace corn {
    Component::Component(Entity& entity): active(true), entity(entity) {}

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
        : Component(entity), location(location), rotation(rotation), zorder(0) {}

    std::pair<Vec2, Deg> CTransform2D::worldTransform() const {
        std::pair<Vec2, Deg> transform = {this->location, this->rotation};
        Entity* ancestor = this->getEntity().getParent();
        while (ancestor) {
            auto* ancestorTransform = ancestor->getComponent<CTransform2D>();
            if (ancestorTransform) {
                transform.second += ancestorTransform->rotation;
                transform.first = ancestorTransform->rotation.rotate(transform.first);
                transform.first += ancestorTransform->location;
            }
            ancestor = ancestor->getParent();
        }
        return transform;
    }

    void CTransform2D::setWorldLocation(Vec2 newLocation) {
        auto [worldLocation, worldRotation] = this->worldTransform();
        Deg parentWorldRotation = worldRotation - this->rotation;
        Vec2 parentWorldLocation = worldLocation - parentWorldRotation.rotate(this->location);
        this->location = (-parentWorldRotation).rotate(newLocation - parentWorldLocation);
    }

    void CTransform2D::addWorldLocationOffset(Vec2 offset) {
        Deg worldRotation = this->worldTransform().second;
        Deg parentWorldRotation = worldRotation - this->rotation;
        this->location += (-parentWorldRotation).rotate(offset);
    }

    int CTransform2D::getZOrder() const {
        return this->zorder;
    }

    void CTransform2D::setZOrder(int _zorder) {
        (void)_zorder;
        this->zorder = _zorder;
        this->getScene().getEventManager().emit(EventArgsZOrderChange(&this->getEntity()));
    }

    CSprite::CSprite(Entity& entity, Image *image, Vec2 topLeft)
        : Component(entity), image(image), topLeft(topLeft), visible(true) {}

    CSprite::~CSprite() {
        delete this->image;
    }

    CMovement2D::CMovement2D(Entity& entity, Vec2 velocity, float angularVelocity)
        : Component(entity), velocity(velocity), angularVelocity(angularVelocity) {}

    std::pair<Vec2, float> CMovement2D::worldMovement() const {
        std::pair<Vec2, float> movement = {this->velocity, this->angularVelocity};
        Entity* ancestor = this->getEntity().getParent();
        while (ancestor) {
            auto* ancestorMovement = ancestor->getComponent<CMovement2D>();
            if (ancestorMovement) {
                movement.second += ancestorMovement->angularVelocity;
                auto* ancestorTransform = ancestor->getComponent<CTransform2D>();
                if (ancestorTransform) {
                    movement.first = ancestorTransform->rotation.rotate(movement.first);
                    movement.first += ancestorMovement->velocity;
                }
            }
            ancestor = ancestor->getParent();
        }
        return movement;
    }

    void CMovement2D::setWorldVelocity(Vec2 newVelocity) {
        // Calculate parent rotation
        Deg parentWorldRotation = 0.0f;
        Entity* current = this->getEntity().getParent();
        while (current) {
            auto currentTransform = current->getComponent<CTransform2D>();
            if (currentTransform) {
                parentWorldRotation += currentTransform->rotation;
            }
            current = current->getParent();
        }

        // Calculate parent world velocity
        Vec2 worldVelocity = this->worldMovement().first;
        Vec2 parentWorldVelocity = worldVelocity - parentWorldRotation.rotate(this->velocity);

        this->velocity = (-parentWorldRotation).rotate(newVelocity - parentWorldVelocity);
    }

    void CMovement2D::addWorldVelocityOffset(Vec2 offset) {
        // Calculate parent rotation
        Deg parentWorldRotation = 0.0f;
        Entity* current = this->getEntity().getParent();
        while (current) {
            auto currentTransform = current->getComponent<CTransform2D>();
            if (currentTransform) {
                parentWorldRotation += currentTransform->rotation;
            }
            current = current->getParent();
        }

        this->velocity += (-parentWorldRotation).rotate(offset);
    }

    CGravity2D::CGravity2D(Entity& entity, float scale): Component(entity), scale(scale) {}

    CAABB::CAABB(Entity& entity, Vec2 ul, Vec2 lr): Component(entity), ul(ul), lr(lr) {}

    bool CAABB::overlapWith(const CAABB& other) const {
        auto* transform1 = this->getEntity().getComponent<CTransform2D>();
        auto* transform2 = other.getEntity().getComponent<CTransform2D>();
        if (!transform1 || !transform2) return false;
        Vec2 worldLocation1 = transform1->worldTransform().first;
        Vec2 worldLocation2 = transform2->worldTransform().first;
        Vec2 ul1 = this->ul + worldLocation1;
        Vec2 lr1 = this->lr + worldLocation1;
        Vec2 ul2 = other.ul + worldLocation2;
        Vec2 lr2 = other.lr + worldLocation2;
        bool xDirection = std::min(lr1.x, lr2.x) > std::max(ul1.x, ul2.x);
        bool yDirection = std::min(lr1.y, lr2.y) > std::max(ul1.y, ul2.y);
        return xDirection && yDirection;
    }

    CCamera::CCamera(Entity& entity, Vec2 anchor, Color background)
        : Component(entity), cameraType(CameraType::_2D), background(background), opacity(255), anchor(anchor.vec3(0)) {

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
        this->viewport.x = Expression(x, units);
        this->viewport.y = Expression(y, units);
        this->viewport.w = Expression(w, units);
        this->viewport.h = Expression(h, units);
    }

    void CCamera::setFov(const std::string& w, const std::string& h) {
        static const std::array<std::string, 3> units = {"px", "%vw", "%vh"};
        this->fovW = Expression(w, units);
        this->fovH = Expression(h, units);
    }
}

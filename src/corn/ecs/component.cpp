#include <cmath>
#include <corn/ecs/component.h>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>
#include <corn/event/event_manager.h>

namespace corn {
    Component::Component(Entity& entity): active(true), entity(entity) {}

    CTransform2D::CTransform2D(Entity &entity, Vec2 location, Deg rotation)
        : Component(entity), location(location), rotation(rotation), zorder(0) {}

    CTransform2D CTransform2D::worldTransform() const {
        CTransform2D transform = *this;
        Entity* ancestor = this->entity.getParent();
        while (ancestor) {
            auto* ancestorTransform = ancestor->getComponent<CTransform2D>();
            if (ancestorTransform) {
                transform.rotation += ancestorTransform->rotation;
                transform.location = ancestorTransform->rotation.rotate(transform.location);
                transform.location += ancestorTransform->location;
            }
            ancestor = ancestor->getParent();
        }
        return transform;
    }

    void CTransform2D::setWorldLocation(Vec2 newLocation) {
        CTransform2D worldTransform = this->worldTransform();
        Deg parentWorldRotation = worldTransform.rotation - this->rotation;
        Vec2 parentWorldLocation = worldTransform.location - parentWorldRotation.rotate(this->location);
        this->location = (-parentWorldRotation).rotate(newLocation - parentWorldLocation);
    }

    void CTransform2D::addWorldLocationOffset(Vec2 offset) {
        CTransform2D worldTransform = this->worldTransform();
        Deg parentWorldRotation = worldTransform.rotation - this->rotation;
        this->location += (-parentWorldRotation).rotate(offset);
    }

    int CTransform2D::getZOrder() const {
        return this->zorder;
    }

    void CTransform2D::setZOrder(int _zorder) {
        this->zorder = _zorder;
        EventManager::instance().emit(EventArgsZOrderChange(&this->entity));
    }

    CSprite::CSprite(Entity& entity, Image *image, Vec2 topLeft)
        : Component(entity), image(image), topLeft(topLeft) {}

    CSprite::~CSprite() {
        delete this->image;
    }

    CMovement2D::CMovement2D(Entity& entity, Vec2 velocity, float angularVelocity)
        : Component(entity), velocity(velocity), angularVelocity(angularVelocity) {}

    CMovement2D CMovement2D::worldMovement() const {
        CMovement2D movement = *this;
        Entity* ancestor = this->entity.getParent();
        while (ancestor) {
            auto* ancestorMovement = ancestor->getComponent<CMovement2D>();
            if (ancestorMovement) {
                movement.angularVelocity += ancestorMovement->angularVelocity;
                auto* ancestorTransform = ancestor->getComponent<CTransform2D>();
                if (ancestorTransform) {
                    movement.velocity = ancestorTransform->rotation.rotate(movement.velocity);
                    movement.velocity += ancestorMovement->velocity;
                }
            }
            ancestor = ancestor->getParent();
        }
        return movement;
    }

    void CMovement2D::setWorldVelocity(Vec2 newVelocity) {
        // Calculate parent rotation
        Deg parentWorldRotation = 0.0f;
        Entity* current = this->entity.getParent();
        while (current) {
            auto currentTransform = current->getComponent<CTransform2D>();
            if (currentTransform) {
                parentWorldRotation += currentTransform->rotation;
            }
            current = current->getParent();
        }

        // Calculate parent world velocity
        CMovement2D worldMovement = this->worldMovement();
        Vec2 parentWorldVelocity = worldMovement.velocity - parentWorldRotation.rotate(this->velocity);

        this->velocity = (-parentWorldRotation).rotate(newVelocity - parentWorldVelocity);
    }

    void CMovement2D::addWorldVelocityOffset(Vec2 offset) {
        // Calculate parent rotation
        Deg parentWorldRotation = 0.0f;
        Entity* current = this->entity.getParent();
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
        auto* transform1 = this->entity.getComponent<CTransform2D>();
        auto* transform2 = other.entity.getComponent<CTransform2D>();
        if (!transform1 || !transform2) return false;
        Vec2 worldLocation1 = transform1->worldTransform().location;
        Vec2 worldLocation2 = transform2->worldTransform().location;
        Vec2 ul1 = this->ul + worldLocation1;
        Vec2 lr1 = this->lr + worldLocation1;
        Vec2 ul2 = other.ul + worldLocation2;
        Vec2 lr2 = other.lr + worldLocation2;
        bool xDirection = std::min(lr1.x, lr2.x) > std::max(ul1.x, ul2.x);
        bool yDirection = std::min(lr1.y, lr2.y) > std::max(ul1.y, ul2.y);
        if (xDirection && yDirection && this->entity.name == "bottom" && other.entity.name == "bird") {
            int x = 1;
        }
        return xDirection && yDirection;
    }

    CCollisionResolve::CCollisionResolve(Entity &entity): Component(entity) {}

    void CCollisionResolve::resolve(CAABB& self, CAABB& other) {}
}

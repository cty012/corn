#include <cmath>
#include <corn/ecs/component.h>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>
#include <corn/event/event_manager.h>

namespace corn {
    Component::Component(Entity& entity) : active(true), entity(entity) {}

    CTransform2D::CTransform2D(Entity &entity, Vec2 location, Deg rotation)
        : Component(entity), location(location), rotation(rotation), zorder(0) {}

    Vec2 CTransform2D::worldLocation() const {
        Vec2 result = this->location;
        Entity* current = this->entity.getParent();
        while (current) {
            auto* transform = current->getComponent<CTransform2D>();
            if (transform) {
                result += transform->location;
            }
            current = current->getParent();
        }
        return result;
    }

    int CTransform2D::getZOrder() const {
        return this->zorder;
    }

    void CTransform2D::setZOrder(int _zorder) {
        this->zorder = _zorder;
        EventManager::instance().emit(EventArgsZOrderChange(&this->entity));
    }

    CSprite::CSprite(Entity& entity, Image *image) : Component(entity), image(image) {}

    CSprite::~CSprite() {
        delete this->image;
    }

    CMovement2D::CMovement2D(Entity& entity, Vec2 velocity) : Component(entity), velocity(velocity) {}

    CGravity2D::CGravity2D(Entity& entity, double scale) : Component(entity), scale(scale) {}

    CAABB::CAABB(Entity& entity, Vec2 ul, Vec2 lr) : Component(entity), ul(ul), lr(lr) {}

    bool CAABB::overlapWith(const CAABB& other) const {
        auto* transform1 = this->entity.getComponent<CTransform2D>();
        auto* transform2 = other.entity.getComponent<CTransform2D>();
        if (!transform1 || !transform2) return false;
        Vec2 worldLocation1 = transform1->worldLocation();
        Vec2 worldLocation2 = transform2->worldLocation();
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

    CCollisionResolve::CCollisionResolve(Entity &entity) : Component(entity) {}

    void CCollisionResolve::resolve(CAABB& self, CAABB& other) {}
}

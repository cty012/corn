#include <corn/ecs/component.h>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>

namespace corn {
    Component::Component(Entity& entity) : active(true), entity(entity) {}

    CTransform2D::CTransform2D(Entity &entity, Vec2 location, Deg rotation)
        : Component(entity), location(location), rotation(rotation), zorder(0) {}

    Vec2 CTransform2D::worldLocation() const {
        Vec2 result = this->location;
        Entity* current = this->entity.getParent();
        while (current) {
            auto* transform = current->getComponent<CTransform2D>();
            if (!transform) break;
            result += transform->location;
            current = current->getParent();
        }
        return result;
    }

    CSprite::CSprite(Entity& entity, Image *image) : Component(entity), image(image) {}

    CSprite::~CSprite() {
        delete this->image;
    }

    CMovement2D::CMovement2D(Entity& entity, Vec2 velocity) : Component(entity), velocity(velocity) {}

    CGravity2D::CGravity2D(Entity& entity, double scale) : Component(entity), scale(scale) {}

    CAABB::CAABB(Entity& entity, Vec2 ul, Vec2 lr) : Component(entity), ul(ul), lr(lr) {}
}

#include <corn/ecs/component.h>
#include <corn/ecs/entity.h>

namespace corn {
    Component::Component(Entity& entity) : active(true), entity(entity) {}

    CTransform2D::CTransform2D(Entity &entity, Vec2 location, Deg rotation)
        : Component(entity), location(location), rotation(rotation), zorder(0) {}

    CSprite::CSprite(Entity& entity, Image *image) : Component(entity), image(image) {}

    CMovement2D::CMovement2D(Entity& entity, Vec2 velocity) : Component(entity), velocity(velocity) {}

    CGravity2D::CGravity2D(Entity& entity, double scale) : Component(entity), scale(scale) {}

    CAABB::CAABB(Entity& entity, Vec2 ul, Vec2 lr) : Component(entity), ul(ul), lr(lr) {}
}

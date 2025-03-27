#pragma once

#include <corn/ecs/component.h>
#include <corn/geometry/vec2.h>

corn::Entity& createCamera(
        corn::EntityManager& entityManager,
        const corn::Vec2& pos);

corn::Entity& createBlock(
        corn::EntityManager& entityManager,
        const corn::Vec2& pos,
        unsigned int size,
        const corn::Color& color);

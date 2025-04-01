#pragma once

#include <corn/ecs/component.h>
#include <corn/geometry/vec.h>

corn::Entity& createCamera(
        corn::EntityManager& entityManager,
        const corn::Vec2f& pos);

corn::Entity& createBlock(
        corn::EntityManager& entityManager,
        const corn::Vec2f& pos,
        unsigned int size,
        const corn::Color& color);

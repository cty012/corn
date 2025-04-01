#pragma once

#include <string>
#include <corn/ecs/entity.h>
#include <corn/geometry/polygon.h>
#include <corn/geometry/vec.h>
#include <corn/util/color.h>

corn::Entity* createRobot(
        corn::EntityManager& entityManager,
        const std::string& name,
        const corn::Polygon& polygon,
        const corn::Vec2f& position,
        const corn::Color& color);

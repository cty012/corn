#pragma once

#include <string>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>
#include <corn/geometry/polygon.h>
#include <corn/geometry/vec2.h>
#include <corn/util/color.h>

corn::Entity* createRobot(
        corn::EntityManager& entityManager,
        const std::string& name,
        const corn::Polygon& polygon,
        const corn::Vec2& position,
        const corn::Color& color);

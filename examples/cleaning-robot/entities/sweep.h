#pragma once

#include <string>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>

corn::Entity* createSweepRegion(
        corn::EntityManager& entityManager,
        const std::string& name,
        const corn::Color& color);

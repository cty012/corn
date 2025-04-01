#include <corn/ecs/component.h>
#include <corn/ecs/entity_manager.h>
#include "robot.h"

corn::Entity* createRobot(
        corn::EntityManager& entityManager,
        const std::string& name,
        const corn::Polygon& polygon,
        const corn::Vec2f& position,
        const corn::Color& color) {

    corn::Entity* robot = &entityManager.createEntity(name);

    // Transform
    robot->addComponent<corn::CTransform2D>(position);

    // Movement
    robot->addComponent<corn::CMovement2D>();

    // Polygon
    robot->addComponent<corn::CPolygon>(polygon, 0.0f, color);

    return robot;
}

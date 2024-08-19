#include <corn/ecs/component.h>
#include "camera.h"

corn::Entity* createCamera(
        corn::EntityManager& entityManager) {

    corn::Entity* camera = &entityManager.createEntity("camera", nullptr);

    // Transform
    camera->addComponent<corn::CTransform2D>(corn::Vec2::ZERO());

    // Camera
    camera->addComponent<corn::CCamera>(corn::Vec2::ZERO(), corn::Color::WHITE());

    return camera;
}

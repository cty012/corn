#include <corn/ecs/component.h>
#include "camera.h"

corn::Entity* createCamera(
        corn::EntityManager& entityManager) {

    corn::Entity* camera = &entityManager.createEntity("camera", nullptr);

    // Transform
    camera->addComponent<corn::CTransform2D>();

    // Camera
    camera->addComponent<corn::CCamera>(corn::CameraType::_2D, corn::Color::WHITE());

    return camera;
}

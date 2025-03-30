#include <corn/ecs/component.h>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>
#include <corn/media/image.h>
#include <corn/util/color.h>
#include "entities.h"

corn::Entity& createCamera(corn::EntityManager& entityManager, const corn::Vec2f& pos) {
    corn::Entity& cameraEnt = entityManager.createEntity("camera");
    cameraEnt.addComponent<corn::CTransform2D>(pos);
    cameraEnt.addComponent<corn::CCamera>(corn::CameraType::_2D, corn::Color::WHITE());
    return cameraEnt;
}

corn::Entity& createBlock(
        corn::EntityManager& entityManager, const corn::Vec2f& pos, unsigned int size, const corn::Color& color) {

    corn::Entity& blockEnt = entityManager.createEntity("block");
    blockEnt.addComponent<corn::CTransform2D>(pos);
    blockEnt.addComponent<corn::CSprite>(new corn::Image(size, size, color), corn::Vec2f::O());
    return blockEnt;
}

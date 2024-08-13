#include <corn/ecs/component.h>
#include "sweep.h"

corn::Entity* createSweepRegion(
        corn::EntityManager& entityManager,
        const std::string& name,
        const corn::Color& color) {

    corn::Entity* sweepRegion = &entityManager.createEntity(name);

    // Transform
    sweepRegion->addComponent<corn::CTransform2D>(corn::Vec2::ZERO())->setZOrder(-1);

    // Polygon
    sweepRegion->addComponent<corn::CPolygon>(corn::Polygon(), 0, color);

    return sweepRegion;
}

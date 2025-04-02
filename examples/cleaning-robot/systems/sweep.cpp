#include <corn/core/scene.h>
#include <corn/geometry/operations.h>
#include <corn/geometry/polygon.h>
#include "sweep.h"
#include "../entities/sweep.h"

SSweep::SSweep(corn::Scene& scene, const std::vector<corn::CPolygon*>& objects) : corn::System(scene) {
    // Create sweep objects for each tracked object
    for (auto object : objects) {
        this->objects_[object] = createSweepRegion(
                this->getScene().getEntityManager(),
                "sweep::" + object->getEntity().getName(),
                corn::Color::parse("#808080"))->getComponent<corn::CPolygon>();
    }
}

void SSweep::update(float millis) {
    for (auto [object, sweepRegion] : objects_) {
        // Get the transform and movement component
        auto* transform = object->getEntity().getComponent<corn::CTransform2D>();
        auto* movement = object->getEntity().getComponent<corn::CMovement2D>();
        if (!transform || !movement) {
            continue;
        }

        // Skip if the object is not moving
        if (movement->velocity.norm() == 0) {
            continue;
        }

        // Sweep the object
        corn::Vec2f displacement = movement->velocity * (millis * 0.001f);
        corn::Polygon newSweepRegion = polygonSweep(object->getPolygon(), displacement);

        // Translate the swept region to the new position
        std::vector<corn::Vec2f> newVertices = newSweepRegion.getVertices();
        std::vector<std::vector<corn::Vec2f>> newHoles = newSweepRegion.getHoles();
        for (size_t i = 0; i < newHoles.size() + 1; i++) {
            std::vector<corn::Vec2f>& vertices = (i == 0) ? newVertices : newHoles[i - 1];
            for (corn::Vec2f& vertex : vertices) {
                vertex += displacement;
            }
        }
        newSweepRegion.setVertices(newVertices, newHoles);

        // Merge the swept region with the sweep object
        std::vector<corn::Polygon> temp = polygonUnion(sweepRegion->getPolygon(), newSweepRegion);
        if (temp.empty()) {
            continue;
        }
        sweepRegion->setPolygon(std::move(temp[0]));

        // Move the object
        transform->translation += displacement;
    }
}

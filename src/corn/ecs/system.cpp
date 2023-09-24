#include <stack>
#include <vector>
#include <corn/ecs/system.h>

namespace corn {
    System::System() : active(true) {}

    void SMovement2D::update(EntityManager& entityManager, unsigned long long int millis) {
        for (Entity* entity : entityManager.getActiveEntities()) {
            auto trans = entity->getComponent<CTransform2D>();
            auto movement = entity->getComponent<CMovement2D>();
            if (trans == nullptr || movement == nullptr) continue;
            trans->location += movement->velocity.mult((double)millis / 1000.0);
        }
    }

    SGravity::SGravity(double scale) : scale(scale) {}

    void SGravity::update(EntityManager& entityManager, unsigned long long int millis) {
        for (Entity* entity : entityManager.getActiveEntities()) {
            auto movement = entity->getComponent<CMovement2D>();
            auto gravity2D = entity->getComponent<CGravity2D>();
            // TODO: gravity 3D
            if (movement == nullptr || gravity2D == nullptr) continue;
            movement->velocity.y += SGravity::g * this->scale * gravity2D->scale * ((double)millis / 1000.0);
        }
    }

    void SCollisionDetection::update(EntityManager& entityManager, unsigned long long int millis) {
        // TODO
    }
}

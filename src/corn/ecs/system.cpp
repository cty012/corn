#include <stack>
#include <vector>
#include <corn/ecs/system.h>

namespace corn {
    System::System() : active(true) {}

    void SMovement2D::update(EntityManager& entityManager, unsigned long long int millis) {
        for (Entity* entity : entityManager.getActiveEntities()) {
            auto trans = entity->getComponent<CTransform2D>();
            auto movement = entity->getComponent<CMovement2D>();
            if (!trans || !movement) continue;
            trans->location += movement->velocity.mult((double)millis / 1000.0);
        }
    }

    SGravity::SGravity(double scale) : scale(scale) {}

    void SGravity::update(EntityManager& entityManager, unsigned long long int millis) {
        for (Entity* entity : entityManager.getActiveEntities()) {
            auto movement = entity->getComponent<CMovement2D>();
            auto gravity2D = entity->getComponent<CGravity2D>();
            // TODO: gravity 3D
            if (!movement || !gravity2D) continue;
            movement->velocity.y += SGravity::g * this->scale * gravity2D->scale * ((double)millis / 1000.0);
        }
    }

    void SCollisionDetection::update(EntityManager& entityManager, unsigned long long int millis) {
        std::vector<Entity*> entities = entityManager.getActiveEntities();
        for (size_t i = 0; i < entities.size(); i++) {
            for (size_t j = i + 1; j < entities.size(); j++) {
                auto* aabb1 = entities[i]->getComponent<CAABB>();
                auto* aabb2 = entities[j]->getComponent<CAABB>();
                if (!aabb1 || !aabb2 || !aabb1->overlapWith(*aabb2)) continue;
                auto* collisionResolve1 = entities[i]->getComponent<CCollisionResolve>();
                auto* collisionResolve2 = entities[j]->getComponent<CCollisionResolve>();
                if (collisionResolve1) collisionResolve1->resolve(*aabb1, *aabb2);
                if (collisionResolve2) collisionResolve2->resolve(*aabb1, *aabb2);
            }
        }
    }
}

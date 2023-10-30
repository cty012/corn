#include <stack>
#include <vector>
#include <corn/ecs/system.h>

namespace corn {
    System::System() : active(true) {}

    System::~System() = default;

    void SMovement2D::update(EntityManager& entityManager, float millis) {
        for (Entity* entity : entityManager.getEntitiesWith<CTransform2D, CMovement2D>()) {
            auto transform = entity->getComponent<CTransform2D>();
            auto movement = entity->getComponent<CMovement2D>();
            transform->addWorldLocationOffset(movement->velocity * (millis / 1000.0f));
            transform->rotation += movement->angularVelocity * (millis / 1000.0f);
        }
    }

    SGravity::SGravity(float scale) : scale(scale) {}

    void SGravity::update(EntityManager& entityManager, float millis) {
        for (Entity* entity : entityManager.getEntitiesWith<CMovement2D, CGravity2D>()) {
            auto movement = entity->getComponent<CMovement2D>();
            auto gravity2D = entity->getComponent<CGravity2D>();
            // TODO: gravity 3D
            movement->addWorldVelocityOffset(
                    Vec2(0, SGravity::g * this->scale * gravity2D->scale * (millis / 1000.0f)));
        }
    }

    void SCollisionDetection::update(EntityManager& entityManager, float millis) {
        std::vector<Entity*> entities = entityManager.getEntitiesWith<CTransform2D, CAABB>();
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

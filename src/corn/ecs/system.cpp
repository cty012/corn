#include <stack>
#include <vector>
#include <corn/ecs/system.h>

namespace corn {
    System::System() : active(true) {}

    System::~System() = default;

    void SMovement2D::update(EntityManager& entityManager, float millis) {
        for (Entity* entity : entityManager.getActiveEntitiesWith<CTransform2D, CMovement2D>()) {
            auto transform = entity->getComponent<CTransform2D>();
            auto movement = entity->getComponent<CMovement2D>();
            if (!transform->active || !movement->active) continue;
            transform->addWorldLocationOffset(movement->velocity * (millis / 1000.0f));
            transform->rotation += movement->angularVelocity * (millis / 1000.0f);
        }
    }

    SGravity::SGravity(float scale) : scale(scale) {}

    void SGravity::update(EntityManager& entityManager, float millis) {
        for (Entity* entity : entityManager.getActiveEntitiesWith<CMovement2D, CGravity2D>()) {
            auto movement = entity->getComponent<CMovement2D>();
            auto gravity2D = entity->getComponent<CGravity2D>();
            // TODO: gravity 3D
            if (!movement->active || !gravity2D->active) continue;
            movement->addWorldVelocityOffset(
                    Vec2(0, SGravity::g * this->scale * gravity2D->scale * (millis / 1000.0f)));
        }
    }

    void SCollisionDetection::update(EntityManager& entityManager, [[maybe_unused]] float millis) {
        std::vector<Entity*> entities = entityManager.getActiveEntitiesWith<CTransform2D, CAABB>();
        for (size_t i = 0; i < entities.size(); i++) {
            for (size_t j = i + 1; j < entities.size(); j++) {
                auto* aabb1 = entities[i]->getComponent<CAABB>();
                auto* aabb2 = entities[j]->getComponent<CAABB>();
                if (!aabb1->active || !aabb2->active || !aabb1->overlapWith(*aabb2)) continue;
                EventManager::instance().emit(EventArgsCollision(aabb1, aabb2));
            }
        }
    }
}

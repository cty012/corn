#include <stack>
#include <vector>
#include <corn/core/scene.h>
#include <corn/ecs/system.h>

namespace corn {
    System::System(Scene& scene) : active(true), scene(scene) {}

    System::~System() = default;

    Scene& System::getScene() const {
        return this->scene;
    }

    const Game* System::getGame() const {
        return this->scene.getGame();
    }

    SMovement2D::SMovement2D(Scene& scene) : System(scene) {}

    void SMovement2D::update(float millis) {
        for (Entity* entity : this->getScene().getEntityManager().getEntitiesWith<CTransform2D, CMovement2D>()) {
            auto transform = entity->getComponent<CTransform2D>();
            auto movement = entity->getComponent<CMovement2D>();
            transform->addWorldLocationOffset(movement->velocity * (millis / 1000.0f));
            transform->rotation += movement->angularVelocity * (millis / 1000.0f);
        }
    }

    SGravity::SGravity(Scene& scene, float scale) : System(scene), scale(scale) {}

    void SGravity::update(float millis) {
        for (Entity* entity : this->getScene().getEntityManager().getEntitiesWith<CMovement2D, CGravity2D>()) {
            auto movement = entity->getComponent<CMovement2D>();
            auto gravity2D = entity->getComponent<CGravity2D>();
            // TODO: gravity 3D
            movement->addWorldVelocityOffset(
                    Vec2(0, SGravity::g * this->scale * gravity2D->scale * (millis / 1000.0f)));
        }
    }

    SCollisionDetection::SCollisionDetection(Scene& scene) : System(scene) {}

    void SCollisionDetection::update(float millis) {
        (void)millis;
        std::vector<Entity*> entities = this->getScene().getEntityManager().getEntitiesWith<CTransform2D, CAABB>();
        for (size_t i = 0; i < entities.size(); i++) {
            for (size_t j = i + 1; j < entities.size(); j++) {
                auto* aabb1 = entities[i]->getComponent<CAABB>();
                auto* aabb2 = entities[j]->getComponent<CAABB>();
                if (!aabb1 || !aabb2 || !aabb1->overlapWith(*aabb2)) continue;
                this->getScene().getEventManager().emit(EventArgsCollision(aabb1, aabb2));
            }
        }
    }
}

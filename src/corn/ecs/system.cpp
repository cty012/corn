#include <vector>
#include <corn/core/scene.h>
#include <corn/ecs/component.h>
#include <corn/ecs/entity_manager.h>
#include <corn/ecs/system.h>
#include <corn/event/event_args.h>

namespace corn {
    System::System(Scene& scene) noexcept : scene_(scene), active_(true) {}

    System::~System() = default;

    bool System::isActive() const noexcept {
        return this->active_;
    }

    void System::setActive(bool active) noexcept {
        this->active_ = active;
    }

    Scene& System::getScene() const noexcept {
        return this->scene_;
    }

    const Game* System::getGame() const noexcept {
        return this->scene_.getGame();
    }

    SMovement2D::SMovement2D(Scene& scene) noexcept : System(scene) {}

    void SMovement2D::update(float millis) {
        for (Entity* entity : this->getScene().getEntityManager().getActiveEntitiesWith<CTransform2D, CMovement2D>()) {
            auto transform = entity->getComponent<CTransform2D>();
            auto movement = entity->getComponent<CMovement2D>();
            if (!transform->active || !movement->active) continue;
            transform->addWorldLocationOffset(movement->velocity * (millis / 1000.0f));
            transform->rotation += movement->angularVelocity * (millis / 1000.0f);
        }
    }

    SGravity::SGravity(Scene& scene, float g) noexcept : System(scene), g(g) {}

    void SGravity::update(float millis) {
        for (Entity* entity : this->getScene().getEntityManager().getActiveEntitiesWith<CMovement2D, CGravity2D>()) {
            auto movement = entity->getComponent<CMovement2D>();
            auto gravity2D = entity->getComponent<CGravity2D>();
            // TODO: gravity 3D
            if (!movement->active || !gravity2D->active) continue;
            movement->addWorldVelocityOffset(
                    Vec2(0, this->g * gravity2D->scale * (millis / 1000.0f)));
        }
    }

    SCollisionDetection::SCollisionDetection(Scene& scene) noexcept : System(scene) {}

    void SCollisionDetection::update(float) {
        std::vector<Entity*> entities = this->getScene().getEntityManager().getActiveEntitiesWith<CTransform2D, CAABB>();
        for (size_t i = 0; i < entities.size(); i++) {
            for (size_t j = i + 1; j < entities.size(); j++) {
                auto* aabb1 = entities[i]->getComponent<CAABB>();
                auto* aabb2 = entities[j]->getComponent<CAABB>();
                if (!aabb1->active || !aabb2->active || !aabb1->overlapWith(*aabb2)) continue;
                this->getScene().getEventManager().emit(EventArgsCollision(aabb1, aabb2));
            }
        }
    }
}

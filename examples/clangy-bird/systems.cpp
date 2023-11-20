#include "components.h"
#include "constants.h"
#include "scenes.h"
#include "systems.h"

WallManager::WallManager(corn::Scene& scene) : corn::System(scene) {}

void WallManager::update([[maybe_unused]] float millis) {
    bool needNewWall = true;
    // Iterate over existing walls
    for (corn::Entity* entity : this->getScene().getEntityManager().getEntitiesWith<Wall>()) {
        auto* transform = entity->getComponent<corn::CTransform2D>();
        float locationX = transform->worldTransform().first.x;
        if ((locationX + WALL_THICKNESS) < 0) {
            entity->destroy();
        }
        if (WIDTH - (locationX + WALL_THICKNESS) < WALL_INTERVAL) {
            needNewWall = false;
        }
    }
    // Create new wall
    if (needNewWall) {
        createWall(this->getScene().getEntityManager(), WIDTH);
    }
}

BirdCollisionResolve::BirdCollisionResolve(corn::Scene& scene) : corn::System(scene), hasCollided(false) {
    this->active = false;
    this->collisionEventID = corn::EventManager::instance().addListener(
            "corn::game::collision", [this](const corn::EventArgs& args) {
                const auto& _args = dynamic_cast<const corn::EventArgsCollision&>(args);
                this->resolve(_args);
            });
}

BirdCollisionResolve::~BirdCollisionResolve() {
    corn::EventManager::instance().removeListener(this->collisionEventID);
}

void BirdCollisionResolve::resolve(const corn::EventArgsCollision& args) {
    if (this->hasCollided) return;
    if (args.collider1->getEntity().name != "bird" && args.collider2->getEntity().name != "bird") return;
    this->hasCollided = true;
    corn::EventManager::instance().emit(corn::EventArgsScene(corn::SceneOperation::REPLACE, new GameScene()));
}

void BirdCollisionResolve::update([[maybe_unused]] float millis) {}

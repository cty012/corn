#include "components.h"
#include "constants.h"
#include "scenes.h"
#include "systems.h"

void WallManager::update(corn::EntityManager& entityManager, [[maybe_unused]] float millis) {
    bool needNewWall = true;
    // Iterate over existing walls
    for (corn::Entity* entity : entityManager.getEntitiesWith<Wall>()) {
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
        createWall(entityManager, WIDTH);
    }
}

BirdCollisionResolve::BirdCollisionResolve(): hasCollided(false) {
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
    if (args.collider1->entity.name != "bird" && args.collider2->entity.name != "bird") return;
    this->hasCollided = true;
    corn::EventManager::instance().emit(corn::EventArgsScene(corn::SceneOperation::REPLACE, new GameScene()));
}

void BirdCollisionResolve::update([[maybe_unused]] corn::EntityManager& entityManager, [[maybe_unused]] float millis) {}

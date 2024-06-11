#include "components.h"
#include "constants.h"
#include "scenes.h"
#include "systems.h"

WallManager::WallManager(corn::Scene& scene) : corn::System(scene) {}

void WallManager::update(float) {
    bool needNewWall = true;
    // Iterate over existing walls
    for (corn::Entity* entity : this->getScene().getEntityManager().getEntitiesWith<Wall>()) {
        auto* transform = entity->getComponent<corn::CTransform2D>();
        float locationX = transform->getWorldTransform().first.x;
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

BirdCollisionResolve::BirdCollisionResolve(corn::Scene& scene) : corn::System(scene), hasCollided_(false) {
    this->setActive(false);
    this->eventScope_.addListener(
            this->getScene().getEventManager(),
            "corn::game::collision",
            [this](const corn::EventArgs& args) {
                const auto& _args = dynamic_cast<const corn::EventArgsCollision&>(args);
                this->resolve(_args);
            });
}

BirdCollisionResolve::~BirdCollisionResolve() = default;

void BirdCollisionResolve::resolve(const corn::EventArgsCollision& args) {
    if (this->hasCollided_) return;
    if (args.collider1->getEntity().getName() != "bird" && args.collider2->getEntity().getName() != "bird") return;
    this->hasCollided_ = true;
    corn::EventManager::instance().emit(corn::EventArgsScene(
            corn::SceneOperation::REPLACE, new GameScene()));
}

void BirdCollisionResolve::update(float) {}

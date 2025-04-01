#include <corn/core/game.h>
#include <corn/ecs/component.h>
#include <corn/ecs/entity_manager.h>
#include <corn/event/event_args.h>
#include "control.h"

SControl::SControl(corn::Scene& scene, corn::Entity::EntityID robotID)
        : System(scene), robotID_(robotID) {

    this->eventScope_.addListener(
            this->getScene().getEventManager(),
            "corn::input::keyboard",
            [this](const corn::EventArgs& args) {
                const auto& args_ = dynamic_cast<const corn::EventArgsKeyboard&>(args);
                switch (args_.key) {
                    case corn::Key::W:
                    case corn::Key::A:
                    case corn::Key::S:
                    case corn::Key::D:
                    case corn::Key::UP:
                    case corn::Key::LEFT:
                    case corn::Key::DOWN:
                    case corn::Key::RIGHT:
                        this->updateMovement();
                        break;
                    default:
                        break;
                }
            });
}

void SControl::update(float) {}

void SControl::updateMovement() {
    // Get velocity
    corn::Vec2f velocity;
    if (this->getGame()->isPressed(corn::Key::W) || this->getGame()->isPressed(corn::Key::UP)) {
        velocity.y -= 1.0f;
    }
    if (this->getGame()->isPressed(corn::Key::A) || this->getGame()->isPressed(corn::Key::LEFT)) {
        velocity.x -= 1.0f;
    }
    if (this->getGame()->isPressed(corn::Key::S) || this->getGame()->isPressed(corn::Key::DOWN)) {
        velocity.y += 1.0f;
    }
    if (this->getGame()->isPressed(corn::Key::D) || this->getGame()->isPressed(corn::Key::RIGHT)) {
        velocity.x += 1.0f;
    }

    // Normalize velocity
    if (velocity.norm() > 0) {
        velocity = velocity.normalize();
    }

    // Move robot
    this->getScene()
            .getEntityManager()
            .getEntityByID(this->robotID_)
            ->getComponent<corn::CMovement2D>()->velocity = velocity * this->speed_;
}

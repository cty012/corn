#include <corn/ecs/entity_manager.h>
#include <corn/event/event_args.h>
#include <corn/event/event_manager.h>
#include "entities.h"
#include "scenes.h"

MainScene::MainScene() : corn::Scene() {
    // Camera
    this->cameraID_ = createCamera(this->getEntityManager(), corn::Vec2f::O()).getID();

    for (int i = -10; i <= 10; i++) {
        for (int j = -10; j <= 10; j++) {
            // Create chess-like pattern
            createBlock(
                    this->getEntityManager(),
                    corn::Vec2f(static_cast<float>(i) * 100.0f, static_cast<float>(j) * 100.0f),
                    100,
                    (i + j) % 2 == 0 ? corn::Color::WHITE() : corn::Color::parse("#8080ff"));
        }
    }

    // Central block
    createBlock(this->getEntityManager(), corn::Vec2f(-10, -10), 20, corn::Color::parse("#ff0000"));

    // Movable block
    this->movableBlockID_ = createBlock(this->getEntityManager(), corn::Vec2f(-10, -10), 20, corn::Color::parse("#00ff00")).getID();
    this->getEntityManager().getEntityByID(this->movableBlockID_)->getComponent<corn::CSprite>()->active = false;

    // Events
    this->eventScope_.addListener(
            this->getEventManager(),
            "corn::input::keyboard",
            [this](const corn::EventArgs& args) {
                const auto& _args = dynamic_cast<const corn::EventArgsKeyboard&>(args);
                if (_args.status != corn::ButtonEvent::DOWN) {
                    return;
                }

                switch (_args.key) {
                    case corn::Key::W:
                        this->move(corn::Vec2f(0, -this->speed_));
                        break;
                    case corn::Key::A:
                        this->move(corn::Vec2f(-this->speed_, 0));
                        break;
                    case corn::Key::S:
                        this->move(corn::Vec2f(0, this->speed_));
                        break;
                    case corn::Key::D:
                        this->move(corn::Vec2f(this->speed_, 0));
                        break;
                    case corn::Key::Q:
                        this->rotate(this->angularSpeed_);
                        break;
                    case corn::Key::E:
                        this->rotate(-this->angularSpeed_);
                        break;
                    case corn::Key::Z:
                        this->scale(this->scaleSpeed_);
                        break;
                    case corn::Key::X:
                        this->scale(1.0f / this->scaleSpeed_);
                        break;
                    default:
                        break;
                }
            });

    this->eventScope_.addListener(
            this->getEventManager(),
            "corn::input::mousebtn",
            [this](const corn::EventArgs& args) {
                const auto& _args = dynamic_cast<const corn::EventArgsMouseButton&>(args);
                if (_args.status != corn::ButtonEvent::DOWN) {
                    return;
                }

                corn::Vec2f worldPos;
                bool found = this->getEntityManager().screenToWorldPosition(_args.mousePos, worldPos);
                if (found) {
                    corn::Entity* movableBlock = this->getEntityManager().getEntityByID(this->movableBlockID_);
                    movableBlock->getComponent<corn::CTransform2D>()->setWorldTranslation(worldPos - corn::Vec2f(10.0f, 10.0f));
                    movableBlock->getComponent<corn::CSprite>()->active = true;
                }
            });
}

void MainScene::move(const corn::Vec2f& displacement) {
    this->getEntityManager().getEntityByID(this->cameraID_)
            ->getComponent<corn::CTransform2D>()->translation += displacement;
}

void MainScene::rotate(corn::Deg angle) {
    // To rotate the world in positive direction, rotate camera in opposite direction
    this->getEntityManager().getEntityByID(this->cameraID_)
            ->getComponent<corn::CTransform2D>()->rotation -= angle;
}

void MainScene::scale(float factor) {
    this->getEntityManager().getEntityByID(this->cameraID_)
            ->getComponent<corn::CCamera>()->scale *= factor;
}

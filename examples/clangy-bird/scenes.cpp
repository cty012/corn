#include <corn/media.h>
#include <corn/ui.h>
#include <corn/util.h>
#include "etities.h"
#include "scenes.h"
#include "systems.h"

GameScene::GameScene(): paused(false) {
    // Camera
    createCamera(this->entityManager);

    // Entities
    this->bird = createBird(this->entityManager);
    this->birdMovement = this->bird->getComponent<corn::CMovement2D>();
    createCeilAndFloor(this->entityManager);

    // Systems
    this->systems.push_back(new corn::SMovement2D());
    this->systems.push_back(new corn::SGravity());
    this->systems.push_back(new corn::SCollisionDetection());
    this->systems.push_back(new WallManager());
    this->systems.push_back(new BirdCollisionResolve());

    // UI
    auto* body = this->uiManager.createWidget<corn::UIWidget>("body", nullptr);
    body->setX("(100%w - min(100%w * 9, 100%h * 16) / 9) / 2");
    body->setY("(100%h - min(100%w * 9, 100%h * 16) / 16) / 2");
    body->setW("min(100%w * 9, 100%h * 16) / 9");
    body->setH("min(100%w * 9, 100%h * 16) / 16");

    corn::TextStyle style = corn::TextStyle(corn::FontManager::instance().get("consolas"), 24);
    auto* label = this->uiManager.createWidget<corn::UILabel>("test", body, corn::RichText()
        .addText(L"test", style));
    label->setX("100%w - 120px");
    label->setY("100%h - 80px");
    label->setW("0px");
    label->setH("0px");

    // Event listeners
    this->keyboardEventID = corn::EventManager::instance().addListener(
            "corn::input::keyboard", [this](const corn::EventArgs &args) {
                this->onKeyboardEvent(dynamic_cast<const corn::EventArgsKeyboard&>(args));
            });
    this->mouseEventID = corn::EventManager::instance().addListener(
            "corn::input::mousebtn", [this](const corn::EventArgs &args) {
                this->onMouseEvent(dynamic_cast<const corn::EventArgsMouseButton&>(args));
            });
}

GameScene::~GameScene() {
    corn::EventManager::instance().removeListener(this->keyboardEventID);
    corn::EventManager::instance().removeListener(this->mouseEventID);
    for (corn::System* system : this->systems) {
        delete system;
    }
}

bool GameScene::isPaused() const {
    return this->paused;
}

void GameScene::togglePause() {
    this->paused = !this->paused;
    for (corn::System* system : this->systems) {
        system->active = !this->paused;
    }
}

void GameScene::onKeyboardEvent(const corn::EventArgsKeyboard& args) {
    if (args.status != corn::ButtonEvent::DOWN) return;
    switch (args.key) {
        case corn::Key::W:
        case corn::Key::UP:
        case corn::Key::SPACE:
            if (!this->paused) {
                this->birdMovement->velocity.y = -700;
            }
            break;
        case corn::Key::ESC:
            this->togglePause();
        default:
            break;
    }
}

void GameScene::onMouseEvent(const corn::EventArgsMouseButton& args) {
    if (!this->paused && args.mouse == corn::Mouse::LEFT && args.status == corn::ButtonEvent::DOWN) {
        this->birdMovement->velocity.y = -700;
    }
}

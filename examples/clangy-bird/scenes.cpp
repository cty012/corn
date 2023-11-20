#include <corn/ui.h>
#include "etities.h"
#include "scenes.h"
#include "systems.h"

MainMenuScene::MainMenuScene() {
    // UI
    auto* body = this->getUIManager().createWidget<corn::UIWidget>("body", nullptr);
    body->setX("(100%pw - min(100%pw * 9, 100%ph * 16) / 9) / 2");
    body->setY("(100%ph - min(100%pw * 9, 100%ph * 16) / 16) / 2");
    body->setW("min(100%pw * 9, 100%ph * 16) / 9");
    body->setH("min(100%pw * 9, 100%ph * 16) / 16");

    corn::TextStyle style = corn::TextStyle(corn::FontManager::instance().get("noto-sans-zh"), 36);
    auto* title = this->getUIManager().createWidget<corn::UILabel>("title", body, corn::RichText()
            .addText(u8"Clangy bird", style));
    title->setX("100%w");
    title->setY("100%h");
    title->background = corn::Color::rgb(0, 0, 0, 128);
}

MainMenuScene::~MainMenuScene() = default;

GameScene::GameScene(): paused(false) {
    // Camera
    createCamera(this->getEntityManager());

    // Entities
    this->bird = createBird(this->getEntityManager());
    this->birdMovement = this->bird->getComponent<corn::CMovement2D>();
    createCeilAndFloor(this->getEntityManager());

    // Systems
    this->systems.push_back(new corn::SMovement2D(*this));
    this->systems.push_back(new corn::SGravity(*this));
    this->systems.push_back(new corn::SCollisionDetection(*this));
    this->systems.push_back(new WallManager(*this));
    this->systems.push_back(new BirdCollisionResolve(*this));

    // UI
    auto* body = this->getUIManager().createWidget<corn::UIWidget>("body", nullptr);
    body->setX("(100%pw - min(100%pw * 9, 100%ph * 16) / 9) / 2");
    body->setY("(100%ph - min(100%pw * 9, 100%ph * 16) / 16) / 2");
    body->setW("min(100%pw * 9, 100%ph * 16) / 9");
    body->setH("min(100%pw * 9, 100%ph * 16) / 16");

    this->pauseMenu = this->getUIManager().createWidget<corn::UIWidget>("pause", body);
    this->pauseMenu->active = false;
    this->pauseMenu->setW("100%pw");
    this->pauseMenu->setH("100%ph");
    this->pauseMenu->background = corn::Color::rgb(0, 0, 0, 128);

    auto* menu = this->getUIManager().createWidget<corn::UIWidget>("menu", this->pauseMenu);
    menu->setX("50%pw - 50%nw");
    menu->setY("50%ph - 50%nh");

    corn::TextStyle style = corn::TextStyle(corn::FontManager::instance().get("noto-sans-zh"), 36);
    this->getUIManager().createWidget<corn::UILabel>("title", menu, corn::RichText()
            .addText(u8"暂停中 ", style)
            .addText(u8"(PAUSED)", style.setColor(corn::Color::RED())));

    // Event listeners
    this->keyboardEventID = this->getEventManager().addListener(
            "corn::input::keyboard", [this](const corn::EventArgs &args) {
                this->onKeyboardEvent(dynamic_cast<const corn::EventArgsKeyboard&>(args));
            });
    this->mouseEventID = this->getEventManager().addListener(
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
    this->pauseMenu->active = this->paused;
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

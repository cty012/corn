#include <corn/ui.h>
#include "etities.h"
#include "scenes.h"
#include "systems.h"
#include "text_manager.h"

void underlineOnHover(corn::UILabel* label) {
    label->getEventManager().addListener(
            "corn::ui::onenter", [label](const corn::EventArgs& args) {
                (void)args;
                corn::setVariant(label->getText().segments[0], corn::FontVariant::UNDERLINE);
            });
    label->getEventManager().addListener(
            "corn::ui::onexit", [label](const corn::EventArgs& args) {
                (void)args;
                corn::setVariant(label->getText().segments[0], corn::FontVariant::REGULAR);
            });
}

MainMenuScene::MainMenuScene() {
    // UI
    auto* body = this->getUIManager().createWidget<corn::UIWidget>("body", nullptr);
    body->setX("(100%pw - min(100%pw * 9, 100%ph * 16) / 9) / 2");
    body->setY("(100%ph - min(100%pw * 9, 100%ph * 16) / 16) / 2");
    body->setW("min(100%pw * 9, 100%ph * 16) / 9");
    body->setH("min(100%pw * 9, 100%ph * 16) / 16");
    body->background = corn::Color::rgb(60, 179, 113);

    auto* contents = this->getUIManager().createWidget<corn::UIWidget>("contents", body);
    contents->setX("200px");
    contents->setY("120px");

    this->getUIManager().createWidget<corn::UILabel>(
            "title", contents, TextManager::instance().getRichText("main-menu-title"));

    // Start button
    auto* start = this->getUIManager().createWidget<corn::UILabel>(
            "start", contents, TextManager::instance().getRichText("main-menu-start"));
    start->setY("110px");
    underlineOnHover(start);
    start->getEventManager().addListener(
            "corn::ui::onclick", [](const corn::EventArgs& args) {
                (void)args;
                corn::EventManager::instance().emit(
                        corn::EventArgsScene(corn::SceneOperation::PUSH, new GameScene()));
            });

    // Settings button
    auto* settings = this->getUIManager().createWidget<corn::UILabel>(
            "settings", contents, TextManager::instance().getRichText("main-menu-settings"));
    settings->setY("155px");
    underlineOnHover(settings);
    settings->getEventManager().addListener(
            "corn::ui::onclick", [](const corn::EventArgs& args) {
                (void)args;
                corn::EventManager::instance().emit(
                        corn::EventArgsScene(corn::SceneOperation::PUSH, new GameScene()));
            });

    // Exit button
    auto* exit = this->getUIManager().createWidget<corn::UILabel>(
            "exit", contents, TextManager::instance().getRichText("main-menu-exit"));
    exit->setY("200px");
    underlineOnHover(exit);
    exit->getEventManager().addListener(
            "corn::ui::onclick", [](const corn::EventArgs& args) {
                (void)args;
                corn::EventManager::instance().emit(corn::EventArgsExit());
            });
}

MainMenuScene::~MainMenuScene() = default;

GameScene::GameScene() : paused(false) {
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
    this->pauseMenu->background = corn::Color::rgb(0, 0, 0, 100);

    auto* menu = this->getUIManager().createWidget<corn::UIWidget>("menu", this->pauseMenu);
    menu->setX("50%pw - 50%nw");
    menu->setY("50%ph - 50%nh - 50px");

    this->getUIManager().createWidget<corn::UILabel>(
            "title", menu, TextManager::instance().getRichText("game-pause-title"));
    auto* cont = this->getUIManager().createWidget<corn::UILabel>(
            "continue", menu, TextManager::instance().getRichText("game-pause-continue"));
    cont->setY("60px");
    underlineOnHover(cont);
    cont->getEventManager().addListener(
            "corn::ui::onclick", [this](const corn::EventArgs& args) {
                (void)args;
                this->togglePause();
            });

    auto* exitToMainMenu = this->getUIManager().createWidget<corn::UILabel>(
            "exit-to-main-menu", menu, TextManager::instance().getRichText("game-pause-exit-to-main-menu"));
    exitToMainMenu->setY("100px");
    underlineOnHover(exitToMainMenu);
    exitToMainMenu->getEventManager().addListener(
            "corn::ui::onclick", [](const corn::EventArgs& args) {
                (void)args;
                corn::EventManager::instance().emit(corn::EventArgsScene(corn::SceneOperation::POP, nullptr));
            });

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
    this->getEventManager().removeListener(this->keyboardEventID);
    this->getEventManager().removeListener(this->mouseEventID);
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

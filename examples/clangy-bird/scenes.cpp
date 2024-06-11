#include <corn/ui.h>
#include "entities.h"
#include "event_args.h"
#include "scenes.h"
#include "systems.h"
#include "text_manager.h"

void underlineOnHover(corn::UILabel& label) {
    label.setClickable(true);
    label.getEventManager().addListener(
            "corn::ui::onenter",
            [&label](const corn::EventArgs&) {
                corn::RichText newText = label.getText();
                newText.segments[0].style.variant = corn::FontVariant::UNDERLINE;
                label.setText(newText);
            });
    label.getEventManager().addListener(
            "corn::ui::onexit",
            [&label](const corn::EventArgs&) {
                corn::RichText newText = label.getText();
                newText.segments[0].style.variant = corn::FontVariant::REGULAR;
                label.setText(newText);
            });
}

MainMenuScene::MainMenuScene() {
    // UI
    auto& body = this->getUIManager().createWidget<corn::UIWidget>("body", nullptr);
    body.setX("(100%pw - min(100%pw * 9, 100%ph * 16) / 9) / 2");
    body.setY("(100%ph - min(100%pw * 9, 100%ph * 16) / 16) / 2");
    body.setW("min(100%pw * 9, 100%ph * 16) / 9");
    body.setH("min(100%pw * 9, 100%ph * 16) / 16");
    body.setBackground(corn::Color::rgb(60, 179, 113));

    auto& contents = this->getUIManager().createWidget<corn::UIWidget>("contents", &body);
    contents.setX("200px");
    contents.setY("120px");

    // Title
    auto& title = this->getUIManager().createWidget<corn::UILabel>(
            "title", &contents, TextManager::instance().getRichText("main-menu-title"));

    // Start button
    auto& start = this->getUIManager().createWidget<corn::UILabel>(
            "start", &contents, TextManager::instance().getRichText("main-menu-start"));
    start.setY("110px");
    underlineOnHover(start);
    start.getEventManager().addListener(
            "corn::ui::onclick",
            [](const corn::EventArgs& args) {
                if (dynamic_cast<const corn::EventArgsUIOnClick&>(args).mousebtnEvent.status != corn::ButtonEvent::UP) {
                    return;
                }
                corn::EventManager::instance().emit(
                        corn::EventArgsScene(corn::SceneOperation::PUSH, new GameScene()));
            });

    // Settings button
    auto& settings = this->getUIManager().createWidget<corn::UILabel>(
            "settings", &contents, TextManager::instance().getRichText("main-menu-settings"));
    settings.setY("155px");
    underlineOnHover(settings);
    settings.getEventManager().addListener(
            "corn::ui::onclick",
            [](const corn::EventArgs& args) {
                if (dynamic_cast<const corn::EventArgsUIOnClick&>(args).mousebtnEvent.status != corn::ButtonEvent::UP) {
                    return;
                }
                corn::EventManager::instance().emit(
                        corn::EventArgsScene(corn::SceneOperation::PUSH, new SettingsScene()));
            });

    // Tutorial button
    auto& tutorial = this->getUIManager().createWidget<corn::UILabel>(
            "tutorial", &contents, TextManager::instance().getRichText("main-menu-tutorial"));
    tutorial.setY("200px");
    underlineOnHover(tutorial);
    tutorial.getEventManager().addListener(
            "corn::ui::onclick",
            [](const corn::EventArgs& args) {
                if (dynamic_cast<const corn::EventArgsUIOnClick&>(args).mousebtnEvent.status != corn::ButtonEvent::UP) {
                    return;
                }
                corn::EventManager::instance().emit(
                        corn::EventArgsScene(corn::SceneOperation::PUSH, new TutorialScene()));
            });

    // Exit button
    auto& exit = this->getUIManager().createWidget<corn::UILabel>(
            "exit", &contents, TextManager::instance().getRichText("main-menu-exit"));
    exit.setY("245px");
    underlineOnHover(exit);
    exit.getEventManager().addListener(
            "corn::ui::onclick",
            [](const corn::EventArgs& args) {
                if (dynamic_cast<const corn::EventArgsUIOnClick&>(args).mousebtnEvent.status != corn::ButtonEvent::UP) {
                    return;
                }
                corn::EventManager::instance().emit(corn::EventArgsExit());
            });

    this->eventScope_.addListener(
            "clangy-bird::langchange",
            [&title, &start, &settings, &tutorial, &exit](const corn::EventArgs&) {
                title.setText(TextManager::instance().getRichText("main-menu-title"));
                start.setText(TextManager::instance().getRichText("main-menu-start"));
                settings.setText(TextManager::instance().getRichText("main-menu-settings"));
                tutorial.setText(TextManager::instance().getRichText("main-menu-tutorial"));
                exit.setText(TextManager::instance().getRichText("main-menu-exit"));
            });
}

MainMenuScene::~MainMenuScene() = default;

SettingsScene::SettingsScene() {
    // UI
    auto& body = this->getUIManager().createWidget<corn::UIWidget>("body", nullptr);
    body.setX("(100%pw - min(100%pw * 9, 100%ph * 16) / 9) / 2");
    body.setY("(100%ph - min(100%pw * 9, 100%ph * 16) / 16) / 2");
    body.setW("min(100%pw * 9, 100%ph * 16) / 9");
    body.setH("min(100%pw * 9, 100%ph * 16) / 16");
    body.setBackground(corn::Color::rgb(60, 179, 113));

    auto& contents = this->getUIManager().createWidget<corn::UIWidget>("contents", &body);
    contents.setX("200px");
    contents.setY("120px");

    // Title
    auto& title = this->getUIManager().createWidget<corn::UILabel>(
            "title", &contents, TextManager::instance().getRichText("settings-title"));

    // Language change button
    auto& langLabel = this->getUIManager().createWidget<corn::UILabel>(
            "lang-label", &contents, TextManager::instance().getRichText("settings-lang"));
    langLabel.setY("110px");
    std::string language = (std::string)TextManager::instance().getSettings("lang");
    auto& lang = this->getUIManager().createWidget<corn::UILabel>(
            "lang", &langLabel, TextManager::instance().getRichText("settings-lang-" + language));
    lang.setX("100%pw + 10px");
    underlineOnHover(lang);
    lang.getEventManager().addListener(
            "corn::ui::onclick",
            [](const corn::EventArgs& args) {
                if (dynamic_cast<const corn::EventArgsUIOnClick&>(args).mousebtnEvent.status != corn::ButtonEvent::UP) {
                    return;
                }
                std::string language = (std::string)TextManager::instance().getSettings("lang");
                TextManager::instance().changeSettings("lang", language == "eng" ? "chn" : "eng");
                TextManager::instance().saveSettings();
                corn::EventManager::instance().emit(EventArgsLanguageChange());
            });

    // Save button
    auto& save = this->getUIManager().createWidget<corn::UILabel>(
            "save", &body, TextManager::instance().getRichText("settings-save"));
    save.setX("100%pw - 100%nw - 200px");
    save.setY("100%ph - 100%nh - 120px");
    underlineOnHover(save);
    save.getEventManager().addListener(
            "corn::ui::onclick",
            [](const corn::EventArgs& args) {
                if (dynamic_cast<const corn::EventArgsUIOnClick&>(args).mousebtnEvent.status != corn::ButtonEvent::UP) {
                    return;
                }
                corn::EventManager::instance().emit(
                        corn::EventArgsScene(corn::SceneOperation::POP, nullptr));
            });

    this->eventScope_.addListener(
            "clangy-bird::langchange",
            [&title, &langLabel, &lang, &save](const corn::EventArgs&) {
                title.setText(TextManager::instance().getRichText("settings-title"));
                langLabel.setText(TextManager::instance().getRichText("settings-lang"));
                std::string language = (std::string)TextManager::instance().getSettings("lang");
                lang.setText(TextManager::instance().getRichText("settings-lang-" + language));
                save.setText(TextManager::instance().getRichText("settings-save"));
            });
}

SettingsScene::~SettingsScene() = default;

TutorialScene::TutorialScene() {
    // UI
    auto& body = this->getUIManager().createWidget<corn::UIWidget>("body", nullptr);
    body.setX("(100%pw - min(100%pw * 9, 100%ph * 16) / 9) / 2");
    body.setY("(100%ph - min(100%pw * 9, 100%ph * 16) / 16) / 2");
    body.setW("min(100%pw * 9, 100%ph * 16) / 9");
    body.setH("min(100%pw * 9, 100%ph * 16) / 16");
    body.setBackground(corn::Color::rgb(60, 179, 113));

    // Title
    auto& title = this->getUIManager().createWidget<corn::UILabel>(
            "title", &body, TextManager::instance().getRichText("tutorial-title"));
    title.setX("200px");
    title.setY("120px");

    // Contents
    auto& contents = this->getUIManager().createWidget<corn::UIWidget>("contents", &body);
    contents.setX("200px");
    contents.setY("200px");
    contents.setW("100%pw - 400px");
    contents.setH("100%ph - 400px");
    auto& background = this->getUIManager().createWidget<corn::UIImage>(
            "title-image", &contents, new corn::Image(
                    50, 50, corn::Color::rgb(128, 128, 128, 128)));
    background.setZOrder(-1);
    background.setW("100%pw");
    background.setH("100%ph");
    auto& text = this->getUIManager().createWidget<corn::UILabel>(
            "text", &contents, TextManager::instance().getRichText("tutorial-contents"));
    text.setW("100%pw");

    // Back button
    auto& back = this->getUIManager().createWidget<corn::UILabel>(
            "back", &body, TextManager::instance().getRichText("tutorial-back"));
    back.setX("100%pw - 100%nw - 200px");
    back.setY("100%ph - 100%nh - 120px");
    underlineOnHover(back);
    back.getEventManager().addListener(
            "corn::ui::onclick",
            [](const corn::EventArgs& args) {
                if (dynamic_cast<const corn::EventArgsUIOnClick&>(args).mousebtnEvent.status != corn::ButtonEvent::UP) {
                    return;
                }
                corn::EventManager::instance().emit(
                        corn::EventArgsScene(corn::SceneOperation::POP, nullptr));
            });

    this->eventScope_.addListener(
            "clangy-bird::langchange",
            [&title, &text, &back](const corn::EventArgs&) {
                title.setText(TextManager::instance().getRichText("tutorial-title"));
                text.setText(TextManager::instance().getRichText("tutorial-text"));
                back.setText(TextManager::instance().getRichText("tutorial-back"));
            });
}

TutorialScene::~TutorialScene() = default;

GameScene::GameScene() : paused_(false), addedSystems_() {
    // Camera
    createCamera(this->getEntityManager());

    // Entities
    this->bird_ = createBird(this->getEntityManager());
    this->birdMovement_ = this->bird_->getComponent<corn::CMovement2D>();
    createCeilAndFloor(this->getEntityManager());

    // Systems
    this->addedSystems_.push_back(this->addSystem<corn::SMovement2D>());
    this->addedSystems_.push_back(this->addSystem<corn::SGravity>());
    this->addedSystems_.push_back(this->addSystem<corn::SCollisionDetection>());
    this->addedSystems_.push_back(this->addSystem<WallManager>());
    this->addedSystems_.push_back(this->addSystem<BirdCollisionResolve>());

    // UI
    auto& body = this->getUIManager().createWidget<corn::UIWidget>("body", nullptr);
    body.setX("(100%pw - min(100%pw * 9, 100%ph * 16) / 9) / 2");
    body.setY("(100%ph - min(100%pw * 9, 100%ph * 16) / 16) / 2");
    body.setW("min(100%pw * 9, 100%ph * 16) / 9");
    body.setH("min(100%pw * 9, 100%ph * 16) / 16");

    this->pauseMenu_ = &this->getUIManager().createWidget<corn::UIWidget>("pause", &body);
    this->pauseMenu_->setActive(false);
    this->pauseMenu_->setW("100%pw");
    this->pauseMenu_->setH("100%ph");
    this->pauseMenu_->setBackground(corn::Color::rgb(0, 0, 0, 100));

    auto& menu = this->getUIManager().createWidget<corn::UIWidget>("menu", this->pauseMenu_);
    menu.setX("50%pw - 50%nw");
    menu.setY("50%ph - 50%nh - 50px");

    auto& title = this->getUIManager().createWidget<corn::UILabel>(
            "title", &menu, TextManager::instance().getRichText("game-pause-title"));
    auto& cont = this->getUIManager().createWidget<corn::UILabel>(
            "continue", &menu, TextManager::instance().getRichText("game-pause-continue"));
    cont.setY("100px");
    underlineOnHover(cont);
    cont.getEventManager().addListener(
            "corn::ui::onclick",
            [this](const corn::EventArgs& args) {
                if (dynamic_cast<const corn::EventArgsUIOnClick&>(args).mousebtnEvent.status != corn::ButtonEvent::UP) {
                    return;
                }
                this->togglePause();
            });

    auto& exitToMainMenu = this->getUIManager().createWidget<corn::UILabel>(
            "exit-to-main-menu", &menu, TextManager::instance().getRichText("game-pause-exit-to-main-menu"));
    exitToMainMenu.setY("150px");
    underlineOnHover(exitToMainMenu);
    exitToMainMenu.getEventManager().addListener(
            "corn::ui::onclick",
            [](const corn::EventArgs& args) {
                if (dynamic_cast<const corn::EventArgsUIOnClick&>(args).mousebtnEvent.status != corn::ButtonEvent::UP) {
                    return;
                }
                corn::EventManager::instance().emit(
                        corn::EventArgsScene(corn::SceneOperation::POP, nullptr));
            });

    // Event listeners
    this->eventScope_.addListener(
            this->getEventManager(),
            "corn::input::keyboard",
            [this](const corn::EventArgs &args) {
                this->onKeyboardEvent(dynamic_cast<const corn::EventArgsKeyboard&>(args));
            });
    this->eventScope_.addListener(
            this->getEventManager(),
            "corn::input::mousebtn",
            [this](const corn::EventArgs &args) {
                this->onMouseEvent(dynamic_cast<const corn::EventArgsMouseButton&>(args));
            });

    this->eventScope_.addListener(
            "clangy-bird::langchange",
            [&title, &cont, &exitToMainMenu](const corn::EventArgs&) {
                title.setText(TextManager::instance().getRichText("game-pause-title"));
                cont.setText(TextManager::instance().getRichText("game-pause-continue"));
                exitToMainMenu.setText(TextManager::instance().getRichText("game-pause-exit-to-main-menu"));
            });
}

GameScene::~GameScene() = default;

bool GameScene::isPaused() const {
    return this->paused_;
}

void GameScene::togglePause() {
    this->paused_ = !this->paused_;
    this->pauseMenu_->setActive(this->paused_);
    for (corn::System* system : this->addedSystems_) {
        system->setActive(!this->paused_);
    }
}

void GameScene::onKeyboardEvent(const corn::EventArgsKeyboard& args) {
    if (args.status != corn::ButtonEvent::DOWN) return;
    switch (args.key) {
        case corn::Key::W:
        case corn::Key::UP:
        case corn::Key::SPACE:
            if (!this->paused_) {
                this->birdMovement_->velocity.y = -700;
            }
            break;
        case corn::Key::ESC:
            this->togglePause();
        default:
            break;
    }
}

void GameScene::onMouseEvent(const corn::EventArgsMouseButton& args) {
    if (!this->paused_ && args.mouse == corn::Mouse::LEFT && args.status == corn::ButtonEvent::DOWN) {
        this->birdMovement_->velocity.y = -700;
    }
}

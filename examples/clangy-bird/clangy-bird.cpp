#include <iostream>
#include <corn/core.h>
#include <corn/ecs.h>
#include <corn/event.h>
#include <corn/media.h>
#include <corn/util.h>

class GameScene : public corn::Scene {
public:
    GameScene() {
        // Entities
        this->bird = &this->entityManager.createEntity("bird");

        // Components
        this->bird->addComponent<corn::CTransform2D>(corn::Vec2(100, 300));
        this->birdMovement = this->bird->addComponent<corn::CMovement2D>(corn::Vec2(0, -500));
        this->bird->addComponent<corn::CGravity2D>();
        this->bird->addComponent<corn::CSprite>(
                new corn::Image(40, 60, corn::Color::rgb(255, 0, 0)));

        // Systems
        this->systems.push_back(new corn::SMovement2D());
        this->systems.push_back(new corn::SGravity(0.5));

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

    ~GameScene() override {
        corn::EventManager::instance().removeListener(this->keyboardEventID);
        corn::EventManager::instance().removeListener(this->mouseEventID);
    }

private:
    corn::Entity* bird;
    corn::CMovement2D* birdMovement;
    corn::EventManager::ListenerID keyboardEventID;
    corn::EventManager::ListenerID mouseEventID;

    void onKeyboardEvent(const corn::EventArgsKeyboard& args) {
        if (args.status != corn::ButtonEvent::DOWN) return;
        switch (args.key) {
            case corn::Key::W:
            case corn::Key::UP:
            case corn::Key::SPACE:
                this->birdMovement->velocity.y = -800;
                break;
            default:
                break;
        }
    }

    void onMouseEvent(const corn::EventArgsMouseButton& args) {
        if (args.mouse == corn::Mouse::LEFT && args.status == corn::ButtonEvent::DOWN)
            this->birdMovement->velocity.y = -800;
    }
};

int main() {
    corn::Config config;
    config.title = "Clangy Bird";
    corn::Game game(new GameScene(), config);
    game.run();
    return 0;
}

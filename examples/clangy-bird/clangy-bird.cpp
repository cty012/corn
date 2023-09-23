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
        this->birdMovement = this->bird->addComponent<corn::CMovement2D>(corn::Vec2(0, -200));
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
    }

    ~GameScene() override {
        corn::EventManager::instance().removeListener(this->keyboardEventID);
    }

private:
    corn::Entity* bird;
    corn::CMovement2D* birdMovement;
    corn::EventManager::ListenerID keyboardEventID;

    void onKeyboardEvent(const corn::EventArgsKeyboard& args) {
        switch (args.key) {
            case corn::Key::W:
                this->birdMovement->velocity.y = -500;
            default:
                break;
        }
    }
};

int main() {
    corn::Config config;
    config.title = "Clangy Bird";
    corn::Game game(new GameScene(), config);
    game.run();
    return 0;
}

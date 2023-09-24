#include <iostream>
#include <random>
#include <corn/core.h>
#include <corn/ecs.h>
#include <corn/event.h>
#include <corn/media.h>
#include <corn/util.h>

constexpr size_t WIDTH = 1280;
constexpr size_t HEIGHT = 720;

corn::Entity* createBird(corn::EntityManager& entityManager) {
    corn::Entity* bird = &entityManager.createEntity("bird");
    bird->addComponent<corn::CTransform2D>(corn::Vec2(100, 300));
    bird->addComponent<corn::CMovement2D>(corn::Vec2(0, -500));
    bird->addComponent<corn::CGravity2D>();
    bird->addComponent<corn::CSprite>(
            new corn::Image(40, 60, corn::Color::rgb(255, 0, 0)));
    return bird;
}

corn::Entity* createWall(corn::EntityManager& entityManager, double margin, unsigned int holeSize) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    auto topSize = (size_t)(margin + (HEIGHT - margin * 2 - holeSize) * dis(gen));
    size_t bottomSize = HEIGHT - topSize - holeSize;

    corn::Entity* wall = &entityManager.createEntity("wall");
    corn::Entity* top = &entityManager.createEntity("top", wall);
    corn::Entity* bottom = &entityManager.createEntity("bottom", wall);

    wall->addComponent<corn::CTransform2D>(corn::Vec2(WIDTH, 0));
    wall->addComponent<corn::CMovement2D>(corn::Vec2(-100, 0));

    top->addComponent<corn::CTransform2D>(corn::Vec2(0, 0));
    top->addComponent<corn::CAABB>(corn::Vec2(0, 0), corn::Vec2(100, (double)topSize));
    top->addComponent<corn::CSprite>(
            new corn::Image(100, topSize, corn::Color::rgb(50, 205, 50)));

    bottom->addComponent<corn::CTransform2D>(corn::Vec2(0, 0));
    bottom->addComponent<corn::CAABB>(corn::Vec2(0, 0), corn::Vec2(100, (double)topSize));
    bottom->addComponent<corn::CSprite>(
            new corn::Image(100, bottomSize, corn::Color::rgb(50, 205, 50)));
    // TODO: local transform
    return wall;
}

class GameScene : public corn::Scene {
public:
    GameScene() {
        // Entities
        this->bird = createBird(this->entityManager);
        this->birdMovement = this->bird->getComponent<corn::CMovement2D>();

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
        for (corn::System* system : this->systems) {
            delete system;
        }
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
    config.width = WIDTH;
    config.height = HEIGHT;
    corn::Game game(new GameScene(), config);
    game.run();
    return 0;
}

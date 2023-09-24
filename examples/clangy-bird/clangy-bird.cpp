#include <iostream>
#include <random>
#include <corn/core.h>
#include <corn/ecs.h>
#include <corn/event.h>
#include <corn/media.h>
#include <corn/util.h>

constexpr size_t WIDTH = 1280;
constexpr size_t HEIGHT = 720;

constexpr size_t BIRD_WIDTH = 50;
constexpr size_t BIRD_HEIGHT = 50;
const corn::Color BIRD_COLOR = corn::Color::rgb(255, 0, 0);

constexpr size_t CEIL_THICKNESS = 40;
const corn::Color CEIL_COLOR = corn::Color::rgb(255, 0, 0);

constexpr double WALL_THICKNESS = 120;
constexpr double WALL_SPEED = 200;
constexpr double WALL_INTERVAL = 300;
const corn::Color WALL_COLOR = corn::Color::rgb(50, 205, 50);

constexpr size_t HOLE_MIN_PADDING = 120;
constexpr size_t HOLE_SIZE = 260;

corn::Entity* createBird(corn::EntityManager& entityManager) {
    corn::Entity* bird = &entityManager.createEntity("bird");
    auto transform = bird->createComponent<corn::CTransform2D>(corn::Vec2(100, 300));
    transform->zorder = 2;
    bird->createComponent<corn::CMovement2D>(corn::Vec2(0, -500));
    bird->createComponent<corn::CGravity2D>();
    bird->createComponent<corn::CSprite>(
            new corn::Image(BIRD_WIDTH, BIRD_HEIGHT, BIRD_COLOR));
    return bird;
}

corn::Entity* createWall(corn::EntityManager& entityManager, double x) {
    // Randomize hole location
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    double topWallSize = HOLE_MIN_PADDING + (HEIGHT - HOLE_MIN_PADDING * 2 - HOLE_SIZE) * dis(gen);
    double bottomWallSize = HEIGHT - topWallSize - HOLE_SIZE;

    // Entities
    corn::Entity* wall = &entityManager.createEntity("wall");
    corn::Entity* top = &entityManager.createEntity("top", wall);
    corn::Entity* bottom = &entityManager.createEntity("bottom", wall);

    // Components of base node
    wall->createComponent<corn::CTransform2D>(corn::Vec2(x, 0));
    wall->createComponent<corn::CMovement2D>(corn::Vec2(-WALL_SPEED, 0));

    // Components of top wall
    top->createComponent<corn::CTransform2D>(corn::Vec2(0, 0));
    top->createComponent<corn::CAABB>(corn::Vec2(0, 0), corn::Vec2(WALL_THICKNESS, (double)topWallSize));
    top->createComponent<corn::CSprite>(new corn::Image(
            WALL_THICKNESS, (unsigned int)topWallSize, WALL_COLOR));

    // Components of bottom wall
    bottom->createComponent<corn::CTransform2D>(corn::Vec2(0, topWallSize + HOLE_SIZE));
    bottom->createComponent<corn::CAABB>(corn::Vec2(0, 0), corn::Vec2(WALL_THICKNESS, (double)bottomWallSize));
    bottom->createComponent<corn::CSprite>(new corn::Image(
            WALL_THICKNESS, (unsigned int)bottomWallSize, WALL_COLOR));

    return wall;
}

class WallManager : public corn::System {
public:
    void update(corn::EntityManager& entityManager, unsigned long long int millis) override {
        bool needNewWall = true;
        // Iterate over existing walls
        for (corn::Entity* entity : entityManager.getActiveEntities()) {
            if (entity->name != "wall") continue;
            auto* transform = entity->getComponent<corn::CTransform2D>();
            double xLocation = transform->worldLocation().x;
            if ((xLocation + WALL_THICKNESS) < 0) {
                entity->destroy();
            }
            if (WIDTH - (xLocation + WALL_THICKNESS) < WALL_INTERVAL) {
                needNewWall = false;
            }
        }
        // Create new wall
        if (needNewWall) {
            createWall(entityManager, WIDTH);
        }
    }
};

class GameScene : public corn::Scene {
public:
    GameScene() {
        // Entities
        this->bird = createBird(this->entityManager);
        this->birdMovement = this->bird->getComponent<corn::CMovement2D>();

        // Systems
        this->systems.push_back(new corn::SMovement2D());
        this->systems.push_back(new corn::SGravity(0.5));
        this->systems.push_back(new WallManager());

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
                this->birdMovement->velocity.y = -700;
                break;
            default:
                break;
        }
    }

    void onMouseEvent(const corn::EventArgsMouseButton& args) {
        if (args.mouse == corn::Mouse::LEFT && args.status == corn::ButtonEvent::DOWN)
            this->birdMovement->velocity.y = -700;
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

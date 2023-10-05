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
const corn::Color CEIL_COLOR = corn::Color::rgb(255, 128, 0);

constexpr double WALL_THICKNESS = 120;
constexpr double WALL_SPEED = 200;
constexpr double WALL_INTERVAL = 300;
const corn::Color WALL_COLOR = corn::Color::rgb(50, 205, 50);

constexpr size_t HOLE_MIN_PADDING = 120;
constexpr size_t HOLE_SIZE = 260;

class GameScene;

/// Component for identifying walls
struct Wall : public corn::Component {
    explicit Wall(corn::Entity& entity): corn::Component(entity) {}
};

/// Custom collision resolve class for bird
struct BirdCollisionResolve : public corn::CCollisionResolve {
    bool hasCollided;

    explicit BirdCollisionResolve(corn::Entity& entity)
        : corn::CCollisionResolve(entity), hasCollided(false) {}

    void resolve(corn::CAABB& self, corn::CAABB& other) override;  // Implemented later
};

corn::Entity* createBird(corn::EntityManager& entityManager) {
    corn::Entity* bird = &entityManager.createEntity("bird");
    auto transform = bird->createComponent<corn::CTransform2D>(corn::Vec2(300, 300));
    transform->zorder = 2;
    bird->createComponent<corn::CMovement2D>(corn::Vec2(0, -500));
    bird->createComponent<corn::CGravity2D>();
    bird->createComponent<corn::CAABB>(corn::Vec2(0, 0), corn::Vec2(BIRD_WIDTH, BIRD_HEIGHT));
    bird->createComponent<corn::CSprite>(
            new corn::Image(BIRD_WIDTH, BIRD_HEIGHT, BIRD_COLOR));
    bird->addComponent<corn::CCollisionResolve>(new BirdCollisionResolve(*bird));
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
    wall->createComponent<Wall>();

    // Components of top wall
    top->createComponent<corn::CTransform2D>(corn::Vec2::ZERO);
    top->createComponent<corn::CAABB>(corn::Vec2::ZERO, corn::Vec2(WALL_THICKNESS, topWallSize));
    top->createComponent<corn::CSprite>(new corn::Image(
            WALL_THICKNESS, (unsigned int)topWallSize, WALL_COLOR));

    // Components of bottom wall
    bottom->createComponent<corn::CTransform2D>(corn::Vec2(0, topWallSize + HOLE_SIZE));
    bottom->createComponent<corn::CAABB>(corn::Vec2::ZERO, corn::Vec2(WALL_THICKNESS, bottomWallSize));
    bottom->createComponent<corn::CSprite>(new corn::Image(
            WALL_THICKNESS, (unsigned int)bottomWallSize, WALL_COLOR));

    return wall;
}

void createCeilAndFloor(corn::EntityManager& entityManager) {
    corn::Entity* ceil = &entityManager.createEntity("ceil");
    corn::Entity* floor = &entityManager.createEntity("floor");

    // Components of ceil
    auto ceilTransform = ceil->createComponent<corn::CTransform2D>(corn::Vec2::ZERO);
    ceilTransform->zorder = 1;
    ceil->createComponent<corn::CAABB>(corn::Vec2::ZERO, corn::Vec2(WIDTH, CEIL_THICKNESS));
    ceil->createComponent<corn::CSprite>(new corn::Image(WIDTH, CEIL_THICKNESS, CEIL_COLOR));

    // Components of floor
    auto floorTransform = floor->createComponent<corn::CTransform2D>(corn::Vec2(0, HEIGHT - CEIL_THICKNESS));
    floorTransform->zorder = 1;
    floor->createComponent<corn::CAABB>(corn::Vec2::ZERO, corn::Vec2(WIDTH, CEIL_THICKNESS));
    floor->createComponent<corn::CSprite>(new corn::Image(WIDTH, CEIL_THICKNESS, CEIL_COLOR));
}

/// A system for managing wall creation and deletion
class WallManager : public corn::System {
public:
    void update(corn::EntityManager& entityManager, double millis) override {
        bool needNewWall = true;
        // Iterate over existing walls
        for (corn::Entity* entity : entityManager.getEntitiesWith<Wall>()) {
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

/// The main game scene
class GameScene : public corn::Scene {
public:
    GameScene() {
        // Entities
        this->bird = createBird(this->entityManager);
        this->birdMovement = this->bird->getComponent<corn::CMovement2D>();
        createCeilAndFloor(this->entityManager);

        // Systems
        this->systems.push_back(new corn::SMovement2D());
        this->systems.push_back(new corn::SGravity());
        this->systems.push_back(new corn::SCollisionDetection());
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

void BirdCollisionResolve::resolve(corn::CAABB& self, corn::CAABB& other) {
    if (this->hasCollided) return;
    this->hasCollided = true;
    corn::EventManager::instance().emit(corn::EventArgsScene(corn::SceneOperation::REPLACE, new GameScene()));
}

int main() {
    corn::Config config;
    config.title = "Clangy Bird";
    config.width = WIDTH;
    config.height = HEIGHT;
    corn::Game game(new GameScene(), config);
    game.run();
    return 0;
}

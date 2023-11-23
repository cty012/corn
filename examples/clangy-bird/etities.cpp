#include <random>
#include "components.h"
#include "constants.h"
#include "etities.h"

corn::Entity* createCamera(corn::EntityManager& entityManager) {
    corn::Entity* camera = &entityManager.createEntity("camera");
    camera->createComponent<corn::CTransform2D>(corn::Vec2((float)WIDTH / 2, (float)HEIGHT / 2));
    auto* ccamera = camera->createComponent<corn::CCamera>(
            corn::Vec2::ZERO(), corn::Color::rgb(60, 179, 113));
    ccamera->setViewport(
            "(100%ww - min(100%ww * 9, 100%wh * 16) / 9) / 2",
            "(100%wh - min(100%ww * 9, 100%wh * 16) / 16) / 2",
            "min(100%ww * 9, 100%wh * 16) / 9",
            "min(100%ww * 9, 100%wh * 16) / 16");
    ccamera->setFov(std::to_string(WIDTH) + "px", std::to_string(HEIGHT) + "px");
    return camera;
}

corn::Entity* createBird(corn::EntityManager& entityManager) {
    corn::Entity* bird = &entityManager.createEntity("bird");
    auto transform = bird->createComponent<corn::CTransform2D>(corn::Vec2(400, 300));
    transform->setZOrder(2);
    bird->createComponent<corn::CMovement2D>(corn::Vec2(0, -500), -180.0f);
    bird->createComponent<corn::CGravity2D>();
    corn::Vec2 bottomRight = corn::Vec2(BIRD_WIDTH * 0.5, BIRD_HEIGHT * 0.5);
    corn::Vec2 topLeft = -bottomRight;
    bird->createComponent<corn::CAABB>(topLeft, bottomRight);
    bird->createComponent<corn::CSprite>(
            new corn::Image(BIRD_WIDTH, BIRD_HEIGHT, BIRD_COLOR), topLeft);
    return bird;
}

corn::Entity* createWall(corn::EntityManager& entityManager, float x) {
    // Randomize hole location
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    float topWallSize = HOLE_MIN_PADDING + (HEIGHT - HOLE_MIN_PADDING * 2 - HOLE_SIZE) * (float)dis(gen);
    float bottomWallSize = HEIGHT - topWallSize - HOLE_SIZE;

    // Entities
    corn::Entity* wall = &entityManager.createEntity("wall");
    corn::Entity* top = &entityManager.createEntity("top", wall);
    corn::Entity* bottom = &entityManager.createEntity("bottom", wall);

    // Components of base node
    wall->createComponent<corn::CTransform2D>(corn::Vec2(x, 0));
    wall->createComponent<corn::CMovement2D>(corn::Vec2(-WALL_SPEED, 0));
    wall->createComponent<Wall>();

    // Components of top wall
    top->createComponent<corn::CTransform2D>(corn::Vec2::ZERO());
    top->createComponent<corn::CAABB>(corn::Vec2::ZERO(), corn::Vec2(WALL_THICKNESS, topWallSize));
    top->createComponent<corn::CSprite>(new corn::Image(
            (unsigned int)WALL_THICKNESS, (unsigned int)topWallSize, WALL_COLOR));

    // Components of bottom wall
    bottom->createComponent<corn::CTransform2D>(corn::Vec2(0, topWallSize + HOLE_SIZE));
    bottom->createComponent<corn::CAABB>(corn::Vec2::ZERO(), corn::Vec2(WALL_THICKNESS, bottomWallSize));
    bottom->createComponent<corn::CSprite>(new corn::Image(
            (unsigned int)WALL_THICKNESS, (unsigned int)bottomWallSize, WALL_COLOR));

    return wall;
}

void createCeilAndFloor(corn::EntityManager& entityManager) {
    corn::Entity* ceil = &entityManager.createEntity("ceil");
    corn::Entity* floor = &entityManager.createEntity("floor");

    // Components of ceil
    auto ceilTransform = ceil->createComponent<corn::CTransform2D>(corn::Vec2::ZERO());
    ceilTransform->setZOrder(1);
    ceil->createComponent<corn::CAABB>(corn::Vec2::ZERO(), corn::Vec2(WIDTH, CEIL_THICKNESS));
    ceil->createComponent<corn::CSprite>(new corn::Image(WIDTH, CEIL_THICKNESS, CEIL_COLOR));

    // Components of floor
    auto floorTransform = floor->createComponent<corn::CTransform2D>(corn::Vec2(0, HEIGHT - CEIL_THICKNESS));
    floorTransform->setZOrder(1);
    floor->createComponent<corn::CAABB>(corn::Vec2::ZERO(), corn::Vec2(WIDTH, CEIL_THICKNESS));
    floor->createComponent<corn::CSprite>(new corn::Image(WIDTH, CEIL_THICKNESS, CEIL_COLOR));
}

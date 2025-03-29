#include <random>
#include <corn/media.h>
#include "components.h"
#include "constants.h"
#include "entities.h"

corn::Entity* createCamera(corn::EntityManager& entityManager) {
    corn::Entity* camera = &entityManager.createEntity("camera");
    camera->addComponent<corn::CTransform2D>(
            corn::Vec2f((float)WIDTH / 2, (float)HEIGHT / 2), corn::Deg(0.0f), corn::Vec2f(1.0f, 1.0f));
    auto* ccamera = camera->addComponent<corn::CCamera>(
            corn::Vec2f::O(), corn::Color::rgb(60, 179, 113));
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
    auto cTransform = bird->addComponent<corn::CTransform2D>(
            corn::Vec2f(400, 300), corn::Deg(0.0f), corn::Vec2f(1.0f, 1.0f));
    cTransform->setZOrder(2);
    bird->addComponent<corn::CMovement2D>(corn::Vec2f(0, -500), -180.0f);
    bird->addComponent<corn::CGravity2D>();
    corn::Vec2f bottomRight(BIRD_WIDTH * 0.5, BIRD_HEIGHT * 0.5);
    corn::Vec2f topLeft = -bottomRight;
    bird->addComponent<corn::CBBox>(topLeft, bottomRight);
    bird->addComponent<corn::CSprite>(
            new corn::Image(BIRD_WIDTH, BIRD_HEIGHT, BIRD_COLOR), topLeft);

    corn::RichText text = corn::RichText()
            .addText("Bird", corn::TextStyle(corn::FontManager::instance().getDefault(), 18));
    bird->addComponent<corn::CText>(corn::TextRender(text), "0px - 50%w", "0px - 50%h");
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
    wall->addComponent<corn::CTransform2D>(
            corn::Vec2f(x, 0), corn::Deg(0.0f), corn::Vec2f(1.0f, 1.0f));
    wall->addComponent<corn::CMovement2D>(corn::Vec2f(-WALL_SPEED, 0));
    wall->addComponent<Wall>();

    // Components of top wall
    top->addComponent<corn::CTransform2D>();
    top->addComponent<corn::CBBox>(corn::Vec2f::O(), corn::Vec2f(WALL_THICKNESS, topWallSize));
    top->addComponent<corn::CSprite>(new corn::Image(
            (unsigned int)WALL_THICKNESS, (unsigned int)topWallSize, WALL_COLOR));

    // Components of bottom wall
    bottom->addComponent<corn::CTransform2D>(
            corn::Vec2f(0, topWallSize + HOLE_SIZE), corn::Deg(0.0f), corn::Vec2f(1.0f, 1.0f));
    bottom->addComponent<corn::CBBox>(corn::Vec2f::O(), corn::Vec2f(WALL_THICKNESS, bottomWallSize));
    bottom->addComponent<corn::CSprite>(new corn::Image(
            (unsigned int)WALL_THICKNESS, (unsigned int)bottomWallSize, WALL_COLOR));

    return wall;
}

void createCeilAndFloor(corn::EntityManager& entityManager) {
    corn::Entity* ceil = &entityManager.createEntity("ceil");
    corn::Entity* floor = &entityManager.createEntity("floor");

    // Components of ceil
    auto ceilTransform = ceil->addComponent<corn::CTransform2D>();
    ceilTransform->setZOrder(1);
    ceil->addComponent<corn::CBBox>(corn::Vec2f::O(), corn::Vec2f(WIDTH, CEIL_THICKNESS));
    ceil->addComponent<corn::CSprite>(new corn::Image(WIDTH, CEIL_THICKNESS, CEIL_COLOR));

    // Components of floor
    auto floorTransform = floor->addComponent<corn::CTransform2D>(
            corn::Vec2f(0, HEIGHT - CEIL_THICKNESS), corn::Deg(0.0f), corn::Vec2f(1.0f, 1.0f));
    floorTransform->setZOrder(1);
    floor->addComponent<corn::CBBox>(corn::Vec2f::O(), corn::Vec2f(WIDTH, CEIL_THICKNESS));
    floor->addComponent<corn::CSprite>(new corn::Image(WIDTH, CEIL_THICKNESS, CEIL_COLOR));
}

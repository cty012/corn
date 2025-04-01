#include "game.h"
#include "../entities/camera.h"
#include "../entities/robot.h"
#include "../systems/control.h"
#include "../systems/sweep.h"

GameScene::GameScene() {
    // Entities
    createCamera(this->getEntityManager());
    corn::Entity* robot = createRobot(
            this->getEntityManager(),
            "robot",
            corn::Polygon::createCircle(corn::Vec2f::O(), 25, 32),
//            corn::Polygon::createRectangle({ -25, -25 }, 50, 50),
            corn::Vec2f::O(),
            corn::Color::parse("#6666cc"));

    // Systems
    this->addSystem<SSweep>(std::vector{ robot->getComponent<corn::CPolygon>() });
    this->addSystem<SControl>(robot->getID());
}

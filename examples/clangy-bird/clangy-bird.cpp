#include <iostream>
#include <corn/core.h>
#include <corn/media.h>
#include <corn/ecs.h>

class GameScene : public corn::Scene {
public:
    corn::Entity* bird;
    GameScene() {
        this->bird = &this->entityManager.createEntity("bird");
        this->bird->addComponent<corn::CTransform2D>(corn::Vec2(100, 300));
        this->bird->addComponent<corn::CGravity>();
        this->bird->addComponent<corn::CSprite>(
                new corn::Image(40, 60, corn::Color::rgb(255, 0, 0)));
    }
};

int main() {
    corn::Config config;
    config.title = "Clangy Bird";
    corn::Game game(new GameScene(), config);
    game.run();
    return 0;
}

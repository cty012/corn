#include <corn/core.h>

class GameScene : public corn::Scene {
public:
    corn::Entity* bird;
    GameScene() {
        this->bird = &this->entityManager.createEntity("bird");
        // TODO
    }
};

int main() {
    corn::Config config;
    config.title = "Clangy Bird";
    corn::Game game(new corn::Scene(), config);
    game.run();
    return 0;
}

#include <corn/core.h>
#include <corn/util.h>
#include "scenes.h"

int main() {
    corn::Config config;
    config.title = "Clangy Bird";
    config.width = WIDTH;
    config.height = HEIGHT;
    corn::Game game(new GameScene(), config);
    game.run();
    return 0;
}

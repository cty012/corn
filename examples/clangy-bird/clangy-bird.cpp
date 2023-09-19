#include <iostream>
#include "corn/core.h"

int main() {
    corn::Config config;
    config.title = "Clangy Bird";
    corn::Game game(new corn::Scene(), config);
    std::cout << "Game start" << std::endl;
    game.run();
    std::cout << "Game over" << std::endl;
    return 0;
}

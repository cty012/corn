#include <iostream>
#include <corn/core.h>

int main() {
    corn::Game game(new corn::Scene());
    std::cout << "Game start" << std::endl;
    game.run();
    std::cout << "Game over" << std::endl;
    return 0;
}

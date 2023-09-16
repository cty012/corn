#pragma once

#include <SFML/Graphics.hpp>
#include <corn/core/input.h>
#include <corn/core/scene.h>
#include <corn/util/config.h>

namespace corn {
    class Interface {
    public:
        explicit Interface(Config* settings);
        ~Interface();
        Interface(const Interface& other);
        Interface& operator=(const Interface& other);

        void init();

        void handleUserInput();

        void clear();
        void render(Scene* scene);
        void update();

    private:
        Config* config;
        sf::RenderWindow* window;
    };

    DisplayMode sfStyle2CornMode(unsigned int style);
    unsigned int cornMode2SfStyle(DisplayMode mode);

    Key sfInput2CornInput(sf::Keyboard::Key key, sf::Keyboard::Scancode code);
    MouseEvent sfInput2CornInput(sf::Mouse::Button button);
    MouseEvent sfInput2CornInput(sf::Mouse::Wheel wheel);
}

#pragma once

#include <SFML/Graphics.hpp>

namespace corn {
    class Interface::InterfaceImpl {
    public:
        sf::RenderWindow* window;

        InterfaceImpl();
        ~InterfaceImpl();
    };
}

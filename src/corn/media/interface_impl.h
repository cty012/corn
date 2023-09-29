#pragma once

#include <SFML/Graphics.hpp>

namespace corn {
    class InterfaceImpl {
    public:
        sf::RenderWindow* window;
        InterfaceImpl();
        ~InterfaceImpl();
    };
}

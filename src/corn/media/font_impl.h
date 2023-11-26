#pragma once

#include <SFML/Graphics.hpp>

namespace corn {
    enum class FontState {
        LOADING, LOADED, LOAD_FAILED
    };

    class Font {
    public:
        FontState state;
        sf::Font sffont;

        Font();
    };
}

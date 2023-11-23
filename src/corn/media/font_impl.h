#pragma once

#include <atomic>
#include <SFML/Graphics.hpp>

namespace corn {
    enum class FontState {
        LOADING, LOADED, LOAD_FAILED
    };
    class Font {
    public:
        Font();
        FontState state;
        sf::Font sffont;
    };
}

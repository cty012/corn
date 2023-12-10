#pragma once

#include <SFML/Graphics.hpp>

namespace corn {
    /**
     * @class FontState
     * @brief Loading status of the font.
     */
    enum class FontState {
        LOADING, LOADED, LOAD_FAILED
    };

    class Font {
    public:
        /// @brief Current load status of the font.
        FontState state;

        /// @brief Stores the actual font data.
        sf::Font sffont;

        /// @brief Constructor.
        Font() noexcept;
    };
}

#pragma once

#include <string>
#include <corn/util/color.h>

namespace sf {
    class Sprite;
}

namespace corn {
    /**
     * @class Image
     * @brief Wrapper around SFML Image.
     */
    class Image {
    public:
        sf::Sprite* sfSprite;
        explicit Image(const std::string& path);
        Image(unsigned int width, unsigned int height, Color color);
        ~Image();
    };
}

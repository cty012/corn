#pragma once

#include <string>
#include <corn/util/color.h>

namespace sf {
    class Texture;
    class Sprite;
}

namespace corn {
    /**
     * @class Image
     * @brief Wrapper around SFML Image.
     */
    class Image {
    public:
        sf::Texture* texture;
        sf::Sprite* sfSprite;
        explicit Image(const std::string& path);  // TODO: allow resizing image
        Image(unsigned int width, unsigned int height, Color color);
        ~Image();
    };
}

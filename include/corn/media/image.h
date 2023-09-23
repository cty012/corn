#pragma once

#include <string>
#include <corn/util/color.h>

namespace sf {
    class Image;
}

namespace corn {
    /**
     * @class Image
     * @brief Wrapper around SFML Image.
     */
    class Image {
    public:
        sf::Image* image;
        explicit Image(const std::string& path);
        Image(unsigned int width, unsigned int height, Color color);
    };
}

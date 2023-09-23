#include <SFML/Graphics.hpp>
#include <corn/media/image.h>
#include <corn/util/exceptions.h>

namespace corn {
    // TODO
    Image::Image(const std::string& path) {
        this->image = new sf::Image();
        if (!this->image->loadFromFile(path))
            throw ResourceLoadFailed("Failed to load image: " + path);
    }

    Image::Image(unsigned int width, unsigned int height, Color color) {
        auto [r, g, b, a] = color.getRGBA();
        this->image = new sf::Image();
        this->image->create(width, height, sf::Color(r, g, b, a));
    }
}

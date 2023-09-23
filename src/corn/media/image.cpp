#include <SFML/Graphics.hpp>
#include <corn/media/image.h>
#include <corn/util/exceptions.h>

namespace corn {
    // TODO
    Image::Image(const std::string& path) {
        sf::Image image = sf::Image();
        if (!image.loadFromFile(path))
            throw ResourceLoadFailed("Failed to load image: " + path + ".");
        this->texture = new sf::Texture();
        if (!texture->loadFromImage(image))
            throw ResourceLoadFailed("Failed to load image: " + path + ".");
        this->sfSprite = new sf::Sprite(*texture);
    }

    Image::Image(unsigned int width, unsigned int height, Color color) {
        auto [r, g, b, a] = color.getRGBA();
        sf::Image image = sf::Image();
        image.create(width, height, sf::Color(r, g, b, a));
        this->texture = new sf::Texture();
        if (!texture->loadFromImage(image))
            throw ResourceLoadFailed("Failed to load image.");
        this->sfSprite = new sf::Sprite(*texture);
    }

    Image::~Image() {
        delete this->sfSprite;
    }
}

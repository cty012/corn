#pragma once

#include <SFML/Graphics.hpp>

namespace corn {
    class ImageImpl {
    public:
        sf::Image image;
        sf::Texture* texture;
        sf::Sprite* sfSprite;

        ImageImpl(const sf::Image& image, const std::string& errorMsg);
        ~ImageImpl();
    };
}

#pragma once

#include <SFML/Graphics.hpp>

namespace corn {
    class ImageImpl {
    public:
        sf::Texture* texture;
        sf::Sprite* sfSprite;

        ImageImpl(const sf::Image& image, const std::string& errorMsg);
        ~ImageImpl();
    };
}

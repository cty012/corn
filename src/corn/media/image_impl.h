#pragma once

#include <SFML/Graphics.hpp>

namespace corn {
    class Image::ImageImpl {
    public:
        /// @brief Stores the actual image data.
        sf::Image image;

        /// @brief The texture is stored on GPU for rendering.
        sf::Texture texture;

        /// @brief The sprite is for applying transformations.
        mutable sf::Sprite sfSprite;

        /**
         * @brief Constructor.
         * @param image The SFML image to wrap around.
         * @param errorMsg Message to display if failed to create texture and sprite.
         */
        ImageImpl(const sf::Image& image, const std::string& errorMsg);

        /// @brief Destructor.
        ~ImageImpl();
    };
}

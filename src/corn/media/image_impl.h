#pragma once

#include <filesystem>
#include <nanosvg.h>
#include <SFML/Graphics.hpp>
#include <corn/geometry/vec.h>

namespace corn {
    enum class ImageType { PNG, JPEG, SVG, UNKNOWN };

    ImageType detectImageType(const std::filesystem::path& path);

    class ImageImpl {
    public:
        /// @brief Path to the image file.
        std::filesystem::path path;

        /// @brief Type of the image (PNG, JPEG, SVG, etc.)
        ImageType type;

        /// @brief Stores the actual image data.
        sf::Image image;

        /// @brief SVG content as a string (if applicable).
        std::string svgContent;

        /// @brief Pointer to the SVG image (if applicable).
        NSVGimage* svgImage;

        /// @brief Scale of the image.
        Vec2f scale;

        /// @brief The texture is stored on GPU for rendering.
        sf::Texture texture;

        /// @brief The sprite is for applying transformations.
        mutable sf::Sprite sfSprite;

        /**
         * @brief Constructor.
         * @param path Path to the image file.
         */
        explicit ImageImpl(const std::filesystem::path& path);

        /**
         * @brief Constructor.
         * @param width Width of the image.
         * @param height Height of the image.
         * @param color Color of the image (RGBA).
         */
        ImageImpl(unsigned int width, unsigned int height, const Color& color);

        /// @brief Destructor.
        ~ImageImpl();

        ImageImpl(const ImageImpl& other);
        ImageImpl& operator=(const ImageImpl& other);

        /// @return Original width of the image.
        float getWidth() const;

        /// @return Original height of the image.
        float getHeight() const;

        bool rasterize(Vec2f extraScale = Vec2f(1.0f, 1.0f), bool useCache = false);
    };
}

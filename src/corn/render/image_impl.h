#pragma once

#include <filesystem>
#include <nanosvg/nanosvg.h>
#include <corn/geometry/vec.h>
#include "bitmap_renderer.h"

namespace corn {
    enum class ImageType {
        PNG,
        JPEG,
        SVG,
        BITMAP,  // Image created from a bitmap, not loaded a file (e.g. color bitmap)
        UNKNOWN,  // Unsupported image format
    };

    ImageType detectImageType(const std::filesystem::path& path);

    class ImageImpl {
    public:
        /// @brief Path to the image file.
        std::filesystem::path path;

        /// @brief Type of the image (PNG, JPEG, SVG, BITMAP, UNKNOWN).
        ImageType type;

        /// @brief Image bitmap data in BGRA8 (applicable for PNG, JPEG, BITMAP).
        std::vector<uint8_t> bitmapData;

        /// @brief Raw size of the image (applicable for PNG, JPEG, BITMAP).
        Vec2u size;

        /// @brief SVG content as a string (applicable for SVG).
        std::string svgContent;

        /// @brief Pointer to the SVG image (applicable for SVG).
        NSVGimage* svgImage;

        /// @brief Scale of the image.
        Vec2f scale;

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
        ImageImpl(uint32_t width, uint32_t height, const Color& color);

        /**
         * @brief Constructor.
         * @param width Width of the image.
         * @param height Height of the image.
         * @param colors Bitmap of the image (RGBA).
         *
         * If the size of colors is less than width * height, the rest of the image will be filled with #00000000.
         */
        ImageImpl(uint32_t width, uint32_t height, const std::vector<Color>& colors);

        /// @brief Destructor.
        ~ImageImpl();

        // Copy and move constructors and assignment operators
        ImageImpl(const ImageImpl& other);
        ImageImpl& operator=(const ImageImpl& other);
        ImageImpl(ImageImpl&& other) noexcept;
        ImageImpl& operator=(ImageImpl&& other) noexcept;

        void destroy();

        /// @return Original width of the image.
        [[nodiscard]] float getWidth() const;

        /// @return Original height of the image.
        [[nodiscard]] float getHeight() const;

        bool rasterize(Vec2f extraScale = Vec2f(1.0f, 1.0f), bool useCache = false);

        [[nodiscard]] BitmapRenderer& getBitmapRenderer();

    private:
        /// @brief The texture is stored on GPU for rendering (applicable for PNG, JPEG, BITMAP).
        BitmapRenderer bitmapRenderer_;
        bool bitmapRendererDirty_ = true;
    };
}

#pragma once

#include <filesystem>
#include <string>
#include <utility>

namespace corn {
    struct Vec2;
    class Color;

    /**
     * @class Image
     * @brief Holds image data.
     *
     * @see Interface
     */
    class Image {
    public:
        friend class Interface;

        /**
         * @brief Loads an image from the path.
         * @param path Path to the image file.
         * @throw ResourceLoadFailed If the file cannot be loaded successfully.
         */
        explicit Image(const std::filesystem::path& path);

        /**
         * @brief Creates an empty image with the provided width, height, and color.
         * @param width Width of the image.
         * @param height Height of the image.
         * @param color Color to fill the image (can be transparent).
         */
        Image(unsigned int width, unsigned int height, Color color);

        /// @brief Destructor.
        ~Image();

        Image(const Image& other);
        Image& operator=(const Image& other);
        Image(Image&& other) noexcept;
        Image& operator=(Image&& other) noexcept;

        /// @return Original width and height of the image (without scaling).
        [[nodiscard]] std::pair<unsigned int, unsigned int> getOriginalSize() const noexcept;

        /// @return Scaled width and height of the image.
        [[nodiscard]] Vec2 getSize() const noexcept;

        /**
         * @brief Resize the image.
         * @param width New width.
         * @param height New height.
         */
        void resize(float width, float height);

    private:
        /// @brief Pimpl idiom.
        class ImageImpl;
        ImageImpl* impl_;
    };
}

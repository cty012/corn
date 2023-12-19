#pragma once

#include <string>
#include <utility>
#include <corn/util/color.h>

namespace corn {
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
        explicit Image(const std::string& path);

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

        /// @return Width and height of the image.
        [[nodiscard]] std::pair<unsigned int, unsigned int> getSize() const noexcept;

        /**
         * @brief Resize the image. Contents will interpolate automatically
         * @param width New width.
         * @param height New height.
         *
         * @todo Implement this function.
         */
        void resize(unsigned int width, unsigned int height);

    private:
        /// @brief Pimpl idiom.
        class ImageImpl;
        ImageImpl* impl_;

        /// @brief Width and height of the image.
        unsigned int width_, height_;
    };
}

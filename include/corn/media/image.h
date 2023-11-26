#pragma once

#include <string>
#include <utility>
#include <corn/util/color.h>

namespace corn {

    /**
     * @class Image
     * @brief Holds image information.
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

        /// @return Width and height of the image.
        [[nodiscard]] std::pair<unsigned int, unsigned int> getSize() const;
        Image& resize(unsigned int width, unsigned int height);  // TODO: allow resizing image

    private:
        class ImageImpl;
        ImageImpl* impl_;
        unsigned int width_, height_;
    };
}

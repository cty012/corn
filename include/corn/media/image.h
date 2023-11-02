#pragma once

#include <string>
#include <corn/util/color.h>

namespace corn {
    /**
     * @class ImageImpl
     * @brief Implementation of the Image class.
     */
    class ImageImpl;

    /**
     * @class Image
     * @brief Holds image information.
     */
    class Image {
    public:
        friend class InterfaceImpl;

        explicit Image(const std::string& path);
        Image(unsigned int width, unsigned int height, Color color);
        ~Image();
        Image(const Image& other);
        Image& operator=(const Image& other);

        [[nodiscard]] std::pair<unsigned int, unsigned int> getSize() const;
        Image& resize(unsigned int width, unsigned int height);  // TODO: allow resizing image

    private:
        ImageImpl* impl;
        unsigned int width, height;
    };
}

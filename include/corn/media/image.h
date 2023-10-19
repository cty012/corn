#pragma once

#include <string>
#include <utility>
#include <corn/util/color.h>

namespace corn {
    /**
     * @class
     * @brief Implementation of the Image class.
     */
    class ImageImpl;

    /**
     * @class Image
     * @brief Holds image information.
     */
    class Image {
    public:
        explicit Image(const std::string& path);  // TODO: allow resizing image
        Image(unsigned int width, unsigned int height, Color color);
        ~Image();

        [[nodiscard]] std::pair<unsigned int, unsigned int> getSize() const;
        Image& resize(unsigned int width, unsigned int height);

        [[nodiscard]] const ImageImpl& impl() const;

    private:
        ImageImpl* imageImpl;
        unsigned int width, height;
    };
}

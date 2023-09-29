#include <corn/media/image.h>
#include <corn/util/exceptions.h>
#include "image_impl.h"

namespace corn {
    ImageImpl::ImageImpl(const sf::Image& image, const std::string& errorMsg) {
        this->texture = new sf::Texture();
        if (!this->texture->loadFromImage(image))
            throw ResourceLoadFailed(errorMsg);
        this->sfSprite = new sf::Sprite(*this->texture);
    }

    ImageImpl::~ImageImpl() {
        delete this->sfSprite;
        delete this->texture;
    }

    Image::Image(const std::string& path) {
        std::string msg = "Failed to load image: " + path + ".";
        sf::Image image = sf::Image();
        if (!image.loadFromFile(path))
            throw ResourceLoadFailed(msg);
        this->imageImpl = new ImageImpl(image, msg);
    }

    Image::Image(unsigned int width, unsigned int height, Color color) {
        std::string msg = "Failed to load image.";
        auto [r, g, b, a] = color.getRGBA();
        sf::Image image = sf::Image();
        image.create(width, height, sf::Color(r, g, b, a));
        this->imageImpl = new ImageImpl(image, msg);
    }

    Image::~Image() {
        delete this->imageImpl;
    }

    const ImageImpl& Image::impl() const {
        return *this->imageImpl;
    }
}

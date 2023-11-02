#include <corn/media/image.h>
#include <corn/util/exceptions.h>
#include "image_impl.h"

namespace corn {
    ImageImpl::ImageImpl(const sf::Image& image, const std::string& errorMsg): image(image) {
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
        auto [w, h] = image.getSize();
        this->width = w;
        this->height = h;
        this->impl = new ImageImpl(image, msg);
    }

    Image::Image(unsigned int width, unsigned int height, Color color): width(width), height(height) {
        std::string msg = "Failed to load image.";
        auto [r, g, b, a] = color.getRGBA();
        sf::Image image = sf::Image();
        image.create(width, height, sf::Color(r, g, b, a));
        this->impl = new ImageImpl(image, msg);
    }

    Image::~Image() {
        delete this->impl;
    }

    Image::Image(const Image& other): width(other.width), height(other.height) {
        this->impl = new ImageImpl(other.impl->image, "Failed to copy image.");
    }

    Image& Image::operator=(const Image& other) {
        if (&other == this) return *this;
        delete this->impl;
        this->width = other.width;
        this->height = other.height;
        this->impl = new ImageImpl(other.impl->image, "Failed to copy image.");
        return *this;
    }

    std::pair<unsigned int, unsigned int> Image::getSize() const {
        return {this->width, this->height};
    }

    Image& Image::resize(unsigned int newWidth, unsigned int newHeight) {
        this->width = newWidth;
        this->height = newHeight;
        return *this;
    }
}

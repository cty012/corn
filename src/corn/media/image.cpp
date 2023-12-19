#include <corn/media/image.h>
#include <corn/util/exceptions.h>
#include "image_impl.h"

namespace corn {
    Image::ImageImpl::ImageImpl(const sf::Image& image, const std::string& errorMsg) : image(image), texture() {
        if (!this->texture.loadFromImage(image))
            throw ResourceLoadFailed(errorMsg);
        this->sfSprite = sf::Sprite(this->texture);
    }

    Image::ImageImpl::~ImageImpl() = default;

    Image::Image(const std::string& path) {
        std::string msg = "Failed to load image: " + path + ".";
        sf::Image image = sf::Image();
        if (!image.loadFromFile(path))
            throw ResourceLoadFailed(msg);
        auto [w, h] = image.getSize();
        this->width_ = w;
        this->height_ = h;
        this->impl_ = new ImageImpl(image, msg);
    }

    Image::Image(unsigned int width, unsigned int height, Color color) : width_(width), height_(height) {
        std::string msg = "Failed to load image.";
        auto [r, g, b, a] = color.getRGBA();
        sf::Image image = sf::Image();
        image.create(width, height, sf::Color(r, g, b, a));
        this->impl_ = new ImageImpl(image, msg);
    }

    Image::~Image() {
        delete this->impl_;
    }

    Image::Image(const Image& other) : width_(other.width_), height_(other.height_) {
        this->impl_ = new ImageImpl(other.impl_->image, "Failed to copy image.");
    }

    Image& Image::operator=(const Image& other) {
        if (&other == this) return *this;
        delete this->impl_;
        this->width_ = other.width_;
        this->height_ = other.height_;
        this->impl_ = new ImageImpl(other.impl_->image, "Failed to copy image.");
        return *this;
    }

    Image::Image(Image&& other) noexcept : width_(other.width_), height_(other.height_) {
        this->impl_ = other.impl_;
        other.impl_ = nullptr;
        other.width_ = other.height_ = 0;
    }

    Image& Image::operator=(Image&& other) noexcept {
        if (&other == this) return *this;
        delete this->impl_;
        this->impl_ = other.impl_;
        other.impl_ = nullptr;
        other.width_ = other.height_ = 0;
        return *this;
    }

    std::pair<unsigned int, unsigned int> Image::getSize() const noexcept {
        return { this->width_, this->height_ };
    }

    void Image::resize(unsigned int newWidth, unsigned int newHeight) {
        if (this->impl_ == nullptr) return;
        this->width_ = newWidth;
        this->height_ = newHeight;
    }
}

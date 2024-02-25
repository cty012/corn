#include <corn/media/image.h>
#include <corn/util/color.h>
#include <corn/util/exceptions.h>
#include "image_impl.h"

namespace corn {
    Image::ImageImpl::ImageImpl(const sf::Image& image, const std::string& errorMsg)
            : image(image), width(image.getSize().x), height(image.getSize().y), scale(1.0f, 1.0f), texture() {

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
        this->impl_ = new ImageImpl(image, msg);
    }

    Image::Image(unsigned int width, unsigned int height, Color color) {
        std::string msg = "Failed to load image.";
        auto [r, g, b, a] = color.getRGBA();
        sf::Image image = sf::Image();
        image.create(width, height, sf::Color(r, g, b, a));
        this->impl_ = new ImageImpl(image, msg);
    }

    Image::~Image() {
        delete this->impl_;
    }

    Image::Image(const Image& other) {
        this->impl_ = new ImageImpl(other.impl_->image, "Failed to copy image.");
    }

    Image& Image::operator=(const Image& other) {
        if (&other == this) return *this;
        delete this->impl_;
        this->impl_ = new ImageImpl(other.impl_->image, "Failed to copy image.");
        return *this;
    }

    Image::Image(Image&& other) noexcept {
        this->impl_ = other.impl_;
        other.impl_ = nullptr;
    }

    Image& Image::operator=(Image&& other) noexcept {
        if (&other == this) return *this;
        delete this->impl_;
        this->impl_ = other.impl_;
        other.impl_ = nullptr;
        return *this;
    }

    std::pair<unsigned int, unsigned int> Image::getOriginalSize() const noexcept {
        return { this->impl_->width, this->impl_->height };
    }

    Vec2 Image::getSize() const noexcept {
        return Vec2((float)this->impl_->width, (float)this->impl_->height) * this->impl_->scale;
    }

    void Image::resize(float width, float height) {
        if (this->impl_ == nullptr) return;
        this->impl_->scale = Vec2(width / this->impl_->width, height / this->impl_->height);
    }
}

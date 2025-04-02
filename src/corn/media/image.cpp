#include <array>
#include <corn/media/image.h>
#include <corn/util/color.h>
#include "../render/image_impl.h"

namespace corn {
    Image::Image(const std::filesystem::path& path) {
        this->impl_ = new ImageImpl(path);
    }

    Image::Image(unsigned int width, unsigned int height, Color color) {
        this->impl_ = new ImageImpl(width, height, color);
    }

    Image::~Image() {
        delete this->impl_;
    }

    Image::Image(const Image& other) {
        this->impl_ = new ImageImpl(*other.impl_);
    }

    Image& Image::operator=(const Image& other) {
        if (&other == this) return *this;
        delete this->impl_;
        this->impl_ = new ImageImpl(*other.impl_);
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

    Vec2f Image::getOriginalSize() const noexcept {
        return Vec2f(this->impl_->getWidth(), this->impl_->getHeight());
    }

    Vec2f Image::getSize() const noexcept {
        return Vec2f(this->impl_->getWidth(), this->impl_->getHeight()) * this->impl_->scale;
    }

    void Image::resize(float width, float height) {
        if (this->impl_ == nullptr) return;
        this->impl_->scale = Vec2f(width / this->impl_->getWidth(), height / this->impl_->getHeight());
    }
}

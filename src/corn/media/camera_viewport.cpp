#include <cmath>
#include <corn/geometry/vec2.h>
#include <corn/media/camera_viewport.h>
#include "camera_viewport_impl.h"

namespace corn {
    CameraViewportImpl::CameraViewportImpl() noexcept : texture() {}

    bool CameraViewportImpl::setSize(const Vec2& size, int antialiasing) {
        unsigned int newWidth = std::lround(size.x);
        unsigned int newHeight = std::lround(size.y);
        auto [width, height] = this->texture.getSize();
        if (width == newWidth && height == newHeight) return false;
        sf::ContextSettings contextSettings;
        contextSettings.antialiasingLevel = antialiasing;
        this->texture.create(newWidth, newHeight, contextSettings);
        return true;
    }

    CameraViewport::CameraViewport() noexcept : x(), y(), w(), h(), impl_(new CameraViewportImpl()) {}

    CameraViewport::~CameraViewport() {
        delete impl_;
    }
}

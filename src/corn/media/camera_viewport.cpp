#include <cmath>
#include <corn/media/camera_viewport.h>
#include "camera_viewport_impl.h"

namespace corn {
    CameraViewportImpl::CameraViewportImpl(): texture() {}

    bool CameraViewportImpl::setSize(const Vec2& size) {
        unsigned int newWidth = std::lround(size.x);
        unsigned int newHeight = std::lround(size.y);
        auto [width, height] = this->texture.getSize();
        if (width == newWidth && height == newHeight) return false;
        sf::ContextSettings contextSettings;
        contextSettings.antialiasingLevel = 16;
        this->texture.create(newWidth, newHeight, contextSettings);
        return true;
    }

    CameraViewport::CameraViewport(): x(), y(), w(), h() {
        impl = new CameraViewportImpl();
    }

    CameraViewport::~CameraViewport() {
        delete impl;
    }
}

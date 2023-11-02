#pragma once

#include <SFML/Graphics.hpp>

namespace corn {
    class InterfaceImpl {
    public:
        sf::RenderWindow* window;
        InterfaceImpl();
        ~InterfaceImpl();
        static CameraViewportImpl* getCameraViewportImpl(const CameraViewport& cameraViewport);
        static ImageImpl* getImageImpl(const Image& image);
    };
}

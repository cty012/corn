#pragma once

#include <SFML/Graphics.hpp>

namespace corn {
    class CameraViewportImpl {
    public:
        CameraViewportImpl();

        /**
         * Set the size of the texture.
         * @param size The target width and height.
         * @return Whether the size changes.
         *
         * The texture is only recreated when the new size doesn't match with the old size.
         */
        bool setSize(const Vec2& size);

        sf::RenderTexture texture;
    };
}

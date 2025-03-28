#pragma once

#include <SFML/Graphics.hpp>
#include <corn/geometry/vec.h>

namespace corn {
    class CameraViewportImpl {
    public:
        CameraViewportImpl() noexcept;

        /**
         * Set the size of the texture.
         * @param size The target width and height.
         * @param antialiasing The antialiasing level.
         * @return Whether the size changes.
         *
         * The texture is only recreated when the new size doesn't match with the old size.
         */
        bool setSize(const Vec2f& size, int antialiasing);

        sf::RenderTexture texture;
    };
}

#pragma once

#include <vector>
#include <bgfx/bgfx.h>
#include "vertex.h"
#include "shader.h"

namespace corn {
    /**
     * @class BitmapRenderer
     * @brief Renders a bitmap texture to the screen.
     *
     * Behaves like a dynamic renderer.
     */
    class BitmapRenderer {
    public:
        BitmapRenderer();

        ~BitmapRenderer();

        void destroy();

        void update(unsigned char* bitmap, float x, float y, uint16_t w, uint16_t h);

        void draw(bgfx::ViewId viewID, const Shader& shader) const;

    private:
        void destroyVertexBuffer();
        void destroyIndexBuffer();
        void destroyTexture();
        void destroySamplerUniform();

        bgfx::DynamicVertexBufferHandle vbf_ = BGFX_INVALID_HANDLE;
        bgfx::IndexBufferHandle ibf_ = BGFX_INVALID_HANDLE;
        bgfx::TextureHandle texture_ = BGFX_INVALID_HANDLE;
        bgfx::UniformHandle samplerUniform_ = BGFX_INVALID_HANDLE;
    };
}

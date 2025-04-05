#pragma once

#include <vector>
#include <bgfx/bgfx.h>
#include "vertex.h"
#include "shader.h"

namespace corn {
    class StaticBitmapRenderer {
    public:
        StaticBitmapRenderer();

        ~StaticBitmapRenderer();

        void destroy();

        void update(unsigned char* bitmap, float x, float y, uint16_t w, uint16_t h);

        void draw(bgfx::ViewId viewId, const Shader& shader) const;

    private:
        void destroyPartial();

        std::vector<TextureVertex2D> vertices_;
        std::vector<uint16_t> indices_;

        bgfx::VertexBufferHandle vbf_ = BGFX_INVALID_HANDLE;
        bgfx::IndexBufferHandle ibf_ = BGFX_INVALID_HANDLE;
        bgfx::TextureHandle texture_ = BGFX_INVALID_HANDLE;
        bgfx::UniformHandle samplerUniform_ = BGFX_INVALID_HANDLE;
    };
}

#pragma once

#include <bgfx/bgfx.h>

namespace corn {
    struct Vertex2D {
        float x, y;

        static const bgfx::VertexLayout& layout();
    };

    struct TextureVertex2D {
        float x, y;
        float u, v;

        static const bgfx::VertexLayout& layout();
    };
}

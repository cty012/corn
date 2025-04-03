#include "vertex.h"

namespace corn {
    const bgfx::VertexLayout& Vertex2D::layout() {
        static const bgfx::VertexLayout layout = []() {
            bgfx::VertexLayout layout;
            layout.begin()
                    .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                    .end();
            return layout;
        }();
        return layout;
    }

    const bgfx::VertexLayout& TextureVertex2D::layout() {
        static const bgfx::VertexLayout layout = []() {
            bgfx::VertexLayout layout;
            layout.begin()
                    .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                    .end();
            return layout;
        }();
        return layout;
    }
}

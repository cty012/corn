#include "bitmap_renderer.h"

namespace corn {
    StaticBitmapRenderer::StaticBitmapRenderer()
            : vbf_(BGFX_INVALID_HANDLE), ibf_(BGFX_INVALID_HANDLE), texture_(BGFX_INVALID_HANDLE) {}

    StaticBitmapRenderer::~StaticBitmapRenderer() {
        this->destroy();
    }

    void StaticBitmapRenderer::destroy() {
        this->destroyPartial();
        if (bgfx::isValid(this->ibf_)) {
            bgfx::destroy(this->ibf_);
            this->ibf_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(this->samplerUniform_)) {
            bgfx::destroy(this->samplerUniform_);
            this->samplerUniform_ = BGFX_INVALID_HANDLE;
        }
    }

    void StaticBitmapRenderer::update(unsigned char* bitmap, float x, float y, uint16_t w, uint16_t h) {
        this->destroyPartial();

        // Create vertex buffer
        this->vertices_ = {
                {x,            y,            0.0f, 0.0f},  // Top-left
                {x + float(w), y,            1.0f, 0.0f},  // Top-right
                {x + float(w), y + float(h), 1.0f, 1.0f},  // Bottom-right
                {x,            y + float(h), 0.0f, 1.0f},  // Bottom-left
        };
        this->vbf_ = bgfx::createVertexBuffer(bgfx::makeRef(
                this->vertices_.data(), sizeof(TextureVertex2D) * this->vertices_.size()), TextureVertex2D::layout());

        // Create index buffer (if not already created)
        if (!bgfx::isValid(this->ibf_)) {
            this->indices_ = {0, 1, 2, 0, 2, 3};
            this->ibf_ = bgfx::createIndexBuffer(
                    bgfx::makeRef(this->indices_.data(), sizeof(u_int16_t) * this->indices_.size()));
        }

        // Create texture
        this->texture_ = bgfx::createTexture2D(
                w, h, false, 1,
                bgfx::TextureFormat::BGRA8,
                0,
                bgfx::copy(bitmap, w * h * 4));

        if (!bgfx::isValid(this->samplerUniform_)) {
            this->samplerUniform_ = bgfx::createUniform("u_sampler", bgfx::UniformType::Sampler);
        }
    }

    void StaticBitmapRenderer::draw(bgfx::ViewId viewId, const Shader& shader) const {
        if (!bgfx::isValid(this->vbf_) || !bgfx::isValid(this->ibf_) || !bgfx::isValid(this->texture_) ||
            !bgfx::isValid(this->samplerUniform_)) {
            return;
        }

        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA | BGFX_STATE_MSAA);
        bgfx::setVertexBuffer(0, this->vbf_);
        bgfx::setIndexBuffer(this->ibf_);
        bgfx::setTexture(0, this->samplerUniform_, this->texture_);
        bgfx::submit(viewId, shader.getProgramHandle());
    }

    void StaticBitmapRenderer::destroyPartial() {
        if (bgfx::isValid(this->vbf_)) {
            bgfx::destroy(this->vbf_);
            this->vbf_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(this->texture_)) {
            bgfx::destroy(this->texture_);
            this->texture_ = BGFX_INVALID_HANDLE;
        }
    }
}

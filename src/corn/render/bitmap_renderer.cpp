#include "bitmap_renderer.h"

namespace corn {
    BitmapRenderer::BitmapRenderer() = default;

    BitmapRenderer::~BitmapRenderer() {
        this->destroy();
    }

    BitmapRenderer::BitmapRenderer(BitmapRenderer&& other) noexcept {
        this->vbf_ = other.vbf_;
        this->ibf_ = other.ibf_;
        this->texture_ = other.texture_;
        this->samplerUniform_ = other.samplerUniform_;

        other.vbf_ = BGFX_INVALID_HANDLE;
        other.ibf_ = BGFX_INVALID_HANDLE;
        other.texture_ = BGFX_INVALID_HANDLE;
        other.samplerUniform_ = BGFX_INVALID_HANDLE;
    }

    BitmapRenderer& BitmapRenderer::operator=(BitmapRenderer&& other) noexcept {
        if (this == &other) return *this;
        this->destroy();

        this->vbf_ = other.vbf_;
        this->ibf_ = other.ibf_;
        this->texture_ = other.texture_;
        this->samplerUniform_ = other.samplerUniform_;

        other.vbf_ = BGFX_INVALID_HANDLE;
        other.ibf_ = BGFX_INVALID_HANDLE;
        other.texture_ = BGFX_INVALID_HANDLE;
        other.samplerUniform_ = BGFX_INVALID_HANDLE;

        return *this;
    }

    void BitmapRenderer::destroy() {
        this->destroyVertexBuffer();
        this->destroyIndexBuffer();
        this->destroyTexture();
        this->destroySamplerUniform();
    }

    void BitmapRenderer::update(const uint8_t* bitmap, float x, float y, uint16_t w, uint16_t h) {
        this->destroy();
        this->destroyTexture();

        // Create a vertex buffer
        std::array vertices = {
                TextureVertex2D{ x,            y,            0.0f, 0.0f },  // Top-left
                TextureVertex2D{ x + float(w), y,            1.0f, 0.0f },  // Top-right
                TextureVertex2D{ x + float(w), y + float(h), 1.0f, 1.0f },  // Bottom-right
                TextureVertex2D{ x,            y + float(h), 0.0f, 1.0f },  // Bottom-left
        };
        const bgfx::Memory* vmem = bgfx::copy(vertices.data(), static_cast<uint32_t>(sizeof(TextureVertex2D) * vertices.size()));
        if (bgfx::isValid(this->vbf_)) {
            bgfx::update(this->vbf_, 0, vmem);
        } else {
            this->vbf_ = bgfx::createDynamicVertexBuffer(vmem, TextureVertex2D::layout());
        }

        // Create an index buffer (if not already created)
        if (!bgfx::isValid(this->ibf_)) {
            std::array<int16_t, 6> indices = { 0, 1, 2, 0, 2, 3 };
            const bgfx::Memory* imem = bgfx::copy(indices.data(), static_cast<uint32_t>(sizeof(uint16_t) * indices.size()));
            this->ibf_ = bgfx::createIndexBuffer(imem);
        }

        // Create texture
        this->texture_ = bgfx::createTexture2D(
                w, h, false, 1,
                bgfx::TextureFormat::BGRA8,
                0,
                bgfx::copy(bitmap, w * h * 4));

        // Create a sampler uniform (if not already created)
        if (!bgfx::isValid(this->samplerUniform_)) {
            this->samplerUniform_ = bgfx::createUniform("u_sampler", bgfx::UniformType::Sampler);
        }
    }

    void BitmapRenderer::draw(bgfx::ViewId viewID, const Shader& shader, const Transform2D& transform) const {
        if (!bgfx::isValid(this->vbf_) || !bgfx::isValid(this->ibf_) || !bgfx::isValid(this->texture_) ||
            !bgfx::isValid(this->samplerUniform_)) {
            return;
        }

        // Transform matrix
        const Mat3f& mat = transform.getMat();
        float mtx[16] = {
                mat[0][0], mat[1][0], 0.0f, mat[2][0],
                mat[0][1], mat[1][1], 0.0f, mat[2][1],
                0.0f,      0.0f,      1.0f, 0.0f,
                mat[0][2], mat[1][2], 0.0f, mat[2][2],
        };

        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA | BGFX_STATE_MSAA);
        bgfx::setVertexBuffer(0, this->vbf_, 0, 4);
        bgfx::setIndexBuffer(this->ibf_, 0, 6);
        bgfx::setTransform(&mtx);
        bgfx::setTexture(0, this->samplerUniform_, this->texture_);
        bgfx::submit(viewID, shader.getProgramHandle());
    }

    void BitmapRenderer::destroyVertexBuffer() {
        if (bgfx::isValid(this->vbf_)) {
            bgfx::destroy(this->vbf_);
            this->vbf_ = BGFX_INVALID_HANDLE;
        }
    }

    void BitmapRenderer::destroyIndexBuffer() {
        if (bgfx::isValid(this->ibf_)) {
            bgfx::destroy(this->ibf_);
            this->ibf_ = BGFX_INVALID_HANDLE;
        }
    }

    void BitmapRenderer::destroyTexture() {
        if (bgfx::isValid(this->texture_)) {
            bgfx::destroy(this->texture_);
            this->texture_ = BGFX_INVALID_HANDLE;
        }
    }

    void BitmapRenderer::destroySamplerUniform() {
        if (bgfx::isValid(this->samplerUniform_)) {
            bgfx::destroy(this->samplerUniform_);
            this->samplerUniform_ = BGFX_INVALID_HANDLE;
        }
    }
}

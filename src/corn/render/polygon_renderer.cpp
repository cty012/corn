#include "polygon_renderer.h"

namespace corn {
    CPolygon::Renderer::~Renderer() noexcept = default;

    StaticPolygonRenderer::StaticPolygonRenderer()
            : vbf_(BGFX_INVALID_HANDLE), edgeIbf_(BGFX_INVALID_HANDLE), fillIbf_(BGFX_INVALID_HANDLE) {

        this->layout_.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .end();
    }

    StaticPolygonRenderer::~StaticPolygonRenderer() {
        if (bgfx::isValid(this->vbf_)) {
            bgfx::destroy(this->vbf_);
            this->vbf_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(this->edgeIbf_)) {
            bgfx::destroy(this->edgeIbf_);
            this->edgeIbf_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(this->fillIbf_)) {
            bgfx::destroy(this->fillIbf_);
            this->fillIbf_ = BGFX_INVALID_HANDLE;
        }
    }

    void StaticPolygonRenderer::update(
            std::vector<Vertex2D> vertices, std::vector<uint16_t> edgeIndices, std::vector<uint32_t> ringSizes,
            std::vector<uint16_t> fillIndices) {

        this->destroy();

        this->vertices_ = std::move(vertices);
        this->edgeIndices_ = std::move(edgeIndices);
        this->ringSizes_ = std::move(ringSizes);
        this->fillIndices_ = std::move(fillIndices);

        this->vbf_ = bgfx::createVertexBuffer(bgfx::makeRef(
                this->vertices_.data(), sizeof(Vertex2D) * this->vertices_.size()), this->layout_);
        this->edgeIbf_ = bgfx::createIndexBuffer(bgfx::makeRef(
                this->edgeIndices_.data(), sizeof(uint16_t) * this->edgeIndices_.size()));
        this->fillIbf_ = bgfx::createIndexBuffer(bgfx::makeRef(
                this->fillIndices_.data(), sizeof(uint16_t) * this->fillIndices_.size()));
    }

    void StaticPolygonRenderer::destroy() noexcept {
        if (bgfx::isValid(this->vbf_)) {
            bgfx::destroy(this->vbf_);
            this->vbf_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(this->edgeIbf_)) {
            bgfx::destroy(this->edgeIbf_);
            this->edgeIbf_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(this->fillIbf_)) {
            bgfx::destroy(this->fillIbf_);
            this->fillIbf_ = BGFX_INVALID_HANDLE;
        }
    }

    void StaticPolygonRenderer::drawEdges(
            bgfx::ViewId viewID, const Shader& shader,
            const Color& color_, float thickness_,
            const Transform2D& transform) const {

        if (!bgfx::isValid(this->vbf_) || !bgfx::isValid(this->edgeIbf_)) {
            return;
        }

        // Color
        const auto [r, g, b, a] = color_.getRGBA();  // NOLINT
        float colorVec[4] = {
                static_cast<float>(r) / 255.0f,
                static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f,
                static_cast<float>(a) / 255.0f,
        };
        bgfx::UniformHandle u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);

        // TODO: thickness is unused in this implementation
        (void)thickness_;

        // Transform matrix
        const Mat3f& mat = transform.getMat();
        float mtx[16] = {
                mat[0][0], mat[0][1], 0.0f, mat[0][2],
                mat[1][0], mat[1][1], 0.0f, mat[1][2],
                0.0f,      0.0f,      1.0f, 0.0f,
                mat[2][0], mat[2][1], 0.0f, mat[2][2],
        };

        // Draw the polygon
        uint32_t start = 0;
        bgfx::setState(
                BGFX_STATE_WRITE_RGB
                | BGFX_STATE_WRITE_A
                | BGFX_STATE_BLEND_ALPHA
                | BGFX_STATE_PT_LINESTRIP
        );
        bgfx::setVertexBuffer(0, this->vbf_);
        bgfx::setUniform(u_color, colorVec);
        bgfx::setTransform(&mtx);
        for (uint32_t count : this->ringSizes_) {
            bgfx::setIndexBuffer(this->edgeIbf_, start, count);
            bgfx::submit(viewID, shader.getProgramHandle(), 0, BGFX_DISCARD_INDEX_BUFFER);

            start += count;
        }

        // Cleanup
        bgfx::destroy(u_color);
    }

    void StaticPolygonRenderer::drawFill(
            bgfx::ViewId viewID, const Shader& shader,
            const Color& color_,
            const Transform2D& transform) const {

        if (!bgfx::isValid(this->vbf_) || !bgfx::isValid(this->fillIbf_)) {
            return;
        }

        // Color
        const auto [r, g, b, a] = color_.getRGBA();  // NOLINT
        float colorVec[4] = {
                static_cast<float>(r) / 255.0f,
                static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f,
                static_cast<float>(a) / 255.0f,
        };
        bgfx::UniformHandle u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);

        // Transform matrix
        const Mat3f& mat = transform.getMat();
        float mtx[16] = {
                mat[0][0], mat[1][0], 0.0f, mat[2][0],
                mat[0][1], mat[1][1], 0.0f, mat[2][1],
                0.0f,      0.0f,      1.0f, 0.0f,
                mat[0][2], mat[1][2], 0.0f, mat[2][2],
        };

        // Draw the polygon
        bgfx::setState(
                BGFX_STATE_WRITE_RGB
                | BGFX_STATE_WRITE_A
                | BGFX_STATE_BLEND_ALPHA
                | BGFX_STATE_PT_TRISTRIP
        );
        bgfx::setVertexBuffer(0, this->vbf_);
        bgfx::setIndexBuffer(this->fillIbf_);
        bgfx::setUniform(u_color, colorVec);
        bgfx::setTransform(&mtx);
        bgfx::submit(viewID, shader.getProgramHandle(), 0, BGFX_DISCARD_ALL);

        // Cleanup
        bgfx::destroy(u_color);
    }

    DynamicPolygonRenderer::DynamicPolygonRenderer()
            : vbf_(BGFX_INVALID_HANDLE), edgeIbf_(BGFX_INVALID_HANDLE), fillIbf_(BGFX_INVALID_HANDLE) {

        this->layout_.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .end();
    }

    DynamicPolygonRenderer::~DynamicPolygonRenderer() noexcept {
        if (bgfx::isValid(this->vbf_)) {
            bgfx::destroy(this->vbf_);
            this->vbf_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(this->edgeIbf_)) {
            bgfx::destroy(this->edgeIbf_);
            this->edgeIbf_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(this->fillIbf_)) {
            bgfx::destroy(this->fillIbf_);
            this->fillIbf_ = BGFX_INVALID_HANDLE;
        }
    }

    void DynamicPolygonRenderer::update(
            std::vector<Vertex2D> vertices, std::vector<uint16_t> edgeIndices, std::vector<uint32_t> ringSizes,
            std::vector<uint16_t> fillIndices) {

        this->vertices_ = std::move(vertices);
        this->edgeIndices_ = std::move(edgeIndices);
        this->ringSizes_ = std::move(ringSizes);
        this->fillIndices_ = std::move(fillIndices);

        const bgfx::Memory* vmem = bgfx::makeRef(this->vertices_.data(), sizeof(Vertex2D) * this->vertices_.size());
        if (bgfx::isValid(this->vbf_)) {
            bgfx::update(this->vbf_, 0, vmem);
        } else {
            this->vbf_ = bgfx::createDynamicVertexBuffer(vmem, this->layout_);
        }

        const bgfx::Memory* emem = bgfx::makeRef(this->edgeIndices_.data(), sizeof(uint16_t) * this->edgeIndices_.size());
        if (bgfx::isValid(this->edgeIbf_)) {
            bgfx::update(this->edgeIbf_, 0, emem);
        } else {
            this->edgeIbf_ = bgfx::createDynamicIndexBuffer(emem);
        }

        const bgfx::Memory* fmem = bgfx::makeRef(this->fillIndices_.data(), sizeof(uint16_t) * this->fillIndices_.size());
        if (bgfx::isValid(this->fillIbf_)) {
            bgfx::update(this->fillIbf_, 0, fmem);
        } else {
            this->fillIbf_ = bgfx::createDynamicIndexBuffer(fmem);
        }
    }

    void DynamicPolygonRenderer::destroy() noexcept {
        if (bgfx::isValid(this->vbf_)) {
            bgfx::destroy(this->vbf_);
            this->vbf_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(this->edgeIbf_)) {
            bgfx::destroy(this->edgeIbf_);
            this->edgeIbf_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(this->fillIbf_)) {
            bgfx::destroy(this->fillIbf_);
            this->fillIbf_ = BGFX_INVALID_HANDLE;
        }
    }

    void DynamicPolygonRenderer::drawEdges(
            bgfx::ViewId viewID, const Shader& shader,
            const Color& color_, float thickness_,
            const Transform2D& transform) const {

        if (!bgfx::isValid(this->vbf_) || !bgfx::isValid(this->edgeIbf_)) {
            return;
        }

        // Color
        const auto [r, g, b, a] = color_.getRGBA();  // NOLINT
        float colorVec[4] = {
                static_cast<float>(r) / 255.0f,
                static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f,
                static_cast<float>(a) / 255.0f,
        };
        bgfx::UniformHandle u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);

        // TODO: thickness is unused in this implementation
        (void)thickness_;

        // Transform matrix
        const Mat3f& mat = transform.getMat();
        float mtx[16] = {
                mat[0][0], mat[0][1], 0.0f, mat[0][2],
                mat[1][0], mat[1][1], 0.0f, mat[1][2],
                0.0f,      0.0f,      1.0f, 0.0f,
                mat[2][0], mat[2][1], 0.0f, mat[2][2],
        };

        // Draw the polygon
        uint32_t start = 0;
        bgfx::setState(
                BGFX_STATE_WRITE_RGB
                | BGFX_STATE_WRITE_A
                | BGFX_STATE_BLEND_ALPHA
                | BGFX_STATE_PT_LINESTRIP
        );
        bgfx::setVertexBuffer(0, this->vbf_);
        bgfx::setUniform(u_color, colorVec);
        bgfx::setTransform(&mtx);
        for (uint32_t count : this->ringSizes_) {
            bgfx::setIndexBuffer(this->edgeIbf_, start, count);
            bgfx::submit(viewID, shader.getProgramHandle(), 0, BGFX_DISCARD_INDEX_BUFFER);

            start += count;
        }

        // Cleanup
        bgfx::destroy(u_color);
    }

    void DynamicPolygonRenderer::drawFill(
            bgfx::ViewId viewID, const Shader& shader,
            const Color& color_,
            const Transform2D& transform) const {

        if (!bgfx::isValid(this->vbf_) || !bgfx::isValid(this->fillIbf_)) {
            return;
        }

        // Color
        const auto [r, g, b, a] = color_.getRGBA();  // NOLINT
        float colorVec[4] = {
                static_cast<float>(r) / 255.0f,
                static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f,
                static_cast<float>(a) / 255.0f,
        };
        bgfx::UniformHandle u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);

        // Transform matrix
        const Mat3f& mat = transform.getMat();
        float mtx[16] = {
                mat[0][0], mat[1][0], 0.0f, mat[2][0],
                mat[0][1], mat[1][1], 0.0f, mat[2][1],
                0.0f,      0.0f,      1.0f, 0.0f,
                mat[0][2], mat[1][2], 0.0f, mat[2][2],
        };

        // Draw the polygon
        bgfx::setState(
                BGFX_STATE_WRITE_RGB
                | BGFX_STATE_WRITE_A
                | BGFX_STATE_BLEND_ALPHA
                | BGFX_STATE_PT_TRISTRIP
        );
        bgfx::setVertexBuffer(0, this->vbf_);
        bgfx::setIndexBuffer(this->fillIbf_);
        bgfx::setUniform(u_color, colorVec);
        bgfx::setTransform(&mtx);
        bgfx::submit(viewID, shader.getProgramHandle(), 0, BGFX_DISCARD_ALL);

        // Cleanup
        bgfx::destroy(u_color);
    }

    void TransientPolygonRenderer::draw(
            bgfx::ViewId viewID, const Shader& shader,
            const std::vector<Vertex2D>& vertices, const std::vector<uint16_t>& indices,
            const Color& color_,
            const Transform2D& transform) {

        // Layout
        static const bgfx::VertexLayout layout = []() {
            bgfx::VertexLayout layout;
            layout.begin()
                    .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                    .end();
            return layout;
        }();

        // Initialize transient buffers
        bgfx::TransientVertexBuffer vbf{};
        bgfx::TransientIndexBuffer ibf{};
        bgfx::allocTransientVertexBuffer(&vbf, vertices.size(), layout);
        bgfx::allocTransientIndexBuffer(&ibf, indices.size());
        memcpy(vbf.data, vertices.data(), sizeof(Vertex2D) * vertices.size());
        memcpy(ibf.data, indices.data(), sizeof(uint16_t) * indices.size());

        // Color
        const auto [r, g, b, a] = color_.getRGBA();  // NOLINT
        float colorVec[4] = {
                static_cast<float>(r) / 255.0f,
                static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f,
                static_cast<float>(a) / 255.0f,
        };
        bgfx::UniformHandle u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);

        // Transform matrix
        const Mat3f& mat = transform.getMat();
        float mtx[16] = {
                mat[0][0], mat[1][0], 0.0f, mat[2][0],
                mat[0][1], mat[1][1], 0.0f, mat[2][1],
                0.0f,      0.0f,      1.0f, 0.0f,
                mat[0][2], mat[1][2], 0.0f, mat[2][2],
        };

        // Set for rendering
        bgfx::setState(
                BGFX_STATE_WRITE_RGB
                | BGFX_STATE_WRITE_A
                | BGFX_STATE_BLEND_ALPHA
                | BGFX_STATE_PT_TRISTRIP);
        bgfx::setVertexBuffer(0, &vbf);
        bgfx::setIndexBuffer(&ibf);
        bgfx::setUniform(u_color, colorVec);
        bgfx::setTransform(&mtx);
        bgfx::submit(viewID, shader.getProgramHandle(), 0, BGFX_DISCARD_ALL);

        // Cleanup
        bgfx::destroy(u_color);
    }
}

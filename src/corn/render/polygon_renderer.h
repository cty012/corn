#pragma once

#include <vector>
#include <bgfx/bgfx.h>
#include <corn/geometry/transform.h>
#include <corn/util/color.h>
#include <corn/ecs/component.h>
#include "shader.h"
#include "vertex.h"

namespace corn {
    class CPolygon::Renderer {
    public:
        virtual ~Renderer() noexcept;

        virtual void update(
                std::vector<Vertex2D> vertices, std::vector<uint16_t> edgeIndices, std::vector<uint32_t> ringSizes,
                std::vector<uint16_t> fillIndices) = 0;

        virtual void destroy() noexcept = 0;

        virtual void drawEdges(
                bgfx::ViewId viewID, const Shader& shader,
                const Color& color_, float thickness_,
                const Transform2D& transform) const = 0;

        virtual void drawFill(
                bgfx::ViewId viewID, const Shader& shader,
                const Color& color_,
                const Transform2D& transform) const = 0;
    };

    class StaticPolygonRenderer : public CPolygon::Renderer {
    public:
        StaticPolygonRenderer();
        ~StaticPolygonRenderer() override;

        void update(
                std::vector<Vertex2D> vertices, std::vector<uint16_t> edgeIndices, std::vector<uint32_t> ringSizes,
                std::vector<uint16_t> fillIndices) override;

        void destroy() noexcept override;

        void drawEdges(
                bgfx::ViewId viewID, const Shader& shader,
                const Color& color_, float thickness_,
                const Transform2D& transform) const override;

        void drawFill(
                bgfx::ViewId viewID, const Shader& shader,
                const Color& color_,
                const Transform2D& transform) const override;

    private:
        std::vector<Vertex2D> vertices_;
        std::vector<uint16_t> edgeIndices_;
        std::vector<uint32_t> ringSizes_;
        std::vector<uint16_t> fillIndices_;

        bgfx::VertexBufferHandle vbf_;
        bgfx::IndexBufferHandle edgeIbf_, fillIbf_;
    };

    class DynamicPolygonRenderer : public CPolygon::Renderer {
    public:
        DynamicPolygonRenderer();
        ~DynamicPolygonRenderer() noexcept override;

        void update(
                std::vector<Vertex2D> vertices, std::vector<uint16_t> edgeIndices, std::vector<uint32_t> ringSizes,
                std::vector<uint16_t> fillIndices) override;

        void destroy() noexcept override;

        void drawEdges(
                bgfx::ViewId viewID, const Shader& shader,
                const Color& color_, float thickness_,
                const Transform2D& transform) const override;

        void drawFill(
                bgfx::ViewId viewID, const Shader& shader,
                const Color& color_,
                const Transform2D& transform) const override;

    private:
        std::vector<Vertex2D> vertices_;
        std::vector<uint16_t> edgeIndices_;
        std::vector<uint32_t> ringSizes_;
        std::vector<uint16_t> fillIndices_;

        bgfx::DynamicVertexBufferHandle vbf_;
        bgfx::DynamicIndexBufferHandle edgeIbf_, fillIbf_;
    };

    class TransientPolygonRenderer {
    public:
        static void draw(
                bgfx::ViewId viewID, const Shader& shader,
                const std::vector<Vertex2D>& vertices, const std::vector<uint16_t>& indices,
                const Color& color_,
                const Transform2D& transform);
    };
}

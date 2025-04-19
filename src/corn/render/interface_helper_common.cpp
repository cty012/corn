#include <bgfx/bgfx.h>
#include <corn/geometry/vec.h>
#include <corn/util/color.h>
#include "image_impl.h"
#include "interface_helper.h"
#include "polygon_renderer.h"

namespace corn {
    void draw(
            bgfx::ViewId viewID,
            const CTransform2D& cTransform, const CSprite& cSprite,
            const Transform2D& worldToCameraTransform, const Shader& bitmapShader) {

        ImageImpl* imageImpl = cSprite.image->impl_;
        Transform2D transform =
                worldToCameraTransform *
                cTransform.getWorldTransform() *
                Transform2D(cSprite.location, 0.0f, imageImpl->scale);

        switch (imageImpl->type) {
            case ImageType::PNG:
            case ImageType::JPEG:
            case ImageType::BITMAP:
                // Draw bitmap
                imageImpl->getBitmapRenderer().draw(viewID, bitmapShader, transform);
                break;
            case ImageType::SVG:
                // todo
                printf("SVG image type not supported yet\n");
                break;
            case ImageType::UNKNOWN:
                printf("Unknown image type\n");
                break;
        }
    }

    void drawLines(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const std::vector<Vec2f>& vertices, float thickness, const Color& color, bool closed,
            const Transform2D& cameraTransform) {

        (void)cCamera; (void)cTransform; (void)vertices; (void)thickness; (void)color; (void)closed; (void)cameraTransform;
        // // Transform
        // Transform2D worldTransform = cameraTransform.inv() * cTransform.getWorldTransform();
        // const Mat3f& mat = worldTransform.getMat();
        // sf::Transform transform {
        //     mat[0][0], mat[0][1], mat[0][2],
        //     mat[1][0], mat[1][1], mat[1][2],
        //     mat[2][0], mat[2][1], mat[2][2],
        // };
        //
        // const auto [r, g, b, a] = color.getRGBA();  // NOLINT
        //
        // for (size_t i = 0; (closed ? i : i + 1) < vertices.size(); i++) {
        //     const Vec2f& start = vertices.at(i);
        //     const Vec2f& end = vertices.at((i + 1) % vertices.size());
        //     Vec2f diff = end - start;
        //     float length = diff.norm();
        //     float angle = std::atan2(diff.y, diff.x) * 180.0f / (float)PI;
        //
        //     sf::RectangleShape line;
        //     line.setSize(sf::Vector2f(length, thickness / worldTransform.mapVector(diff.normalize()).norm()));
        //     line.setOrigin(0, 0);
        //     line.setPosition(start.x, start.y);
        //     line.setFillColor(sf::Color{ r, g, b, a });
        //     line.setRotation(angle);
        //     cCamera.viewport.impl_->texture.draw(line, transform);
        // }
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CLines& cLines,
            const Transform2D& cameraTransform) {

        (void)cCamera; (void)cTransform; (void)cLines; (void)cameraTransform;
        // drawLines(
        //         cCamera,
        //         cTransform, cLines.vertices, cLines.thickness, cLines.color, cLines.closed,
        //         cameraTransform);
    }

    void draw(
            bgfx::ViewId viewID,
            const CTransform2D& cTransform, const CPolygon& cPolygon,
            const Transform2D& worldToCameraTransform, const Shader& polygonShader) {

        CPolygon::Renderer* polygonRenderer = cPolygon.getPolygonRenderer();
        Transform2D transform = worldToCameraTransform * cTransform.getWorldTransform();
        switch (cPolygon.getRenderType()) {
            case CPolygon::RenderType::STATIC:
            case CPolygon::RenderType::DYNAMIC:
                if (cPolygon.thickness > 0) {
                    polygonRenderer->drawEdges(viewID, polygonShader, cPolygon.color, cPolygon.thickness, transform);
                } else {
                    polygonRenderer->drawFill(viewID, polygonShader, cPolygon.color, transform);
                }
                break;
            case CPolygon::RenderType::TRANSIENT: {
                // Vertices
                std::vector<Vertex2D> vertices;
                vertices.reserve(vertices.size());
                for (const Vec2f& vertex : cPolygon.getPolygon().getVerticesFlat()) {
                    vertices.emplace_back(vertex.x, vertex.y);
                }

                // Fill indices
                std::vector<uint16_t> fillIndices;
                const std::vector<size_t>& triangleIndices_ = cPolygon.getPolygon().getTriangleIndices();
                fillIndices.reserve(triangleIndices_.size());
                for (size_t index : triangleIndices_) {
                    fillIndices.push_back(static_cast<uint16_t>(index));
                }

                TransientPolygonRenderer::draw(viewID, polygonShader, vertices, fillIndices, cPolygon.color, transform);
                break;
            }
        }
    }

    void drawUI(
            bgfx::ViewId viewID,
            UIImage& uiImage, float w, float h,
            const Transform2D& transform, const Shader& bitmapShader) {

        const Image* image = uiImage.getImage();
        Vec2f originalSize = image->getOriginalSize();
        Vec2f totalScale(
                originalSize.x != 0.0f ? w / originalSize.x : 1,
                originalSize.y != 0.0f ? h / originalSize.y : 1);
        ImageImpl* imageImpl = uiImage.getImage()->impl_;
        Transform2D totalTransform = transform * Transform2D::dilate(totalScale);

        switch (imageImpl->type) {
            case ImageType::PNG:
            case ImageType::JPEG:
            case ImageType::BITMAP:
                // Draw bitmap
                imageImpl->getBitmapRenderer().draw(viewID, bitmapShader, totalTransform);
                break;
            case ImageType::SVG:
                // todo
                printf("SVG image type not supported yet\n");
                break;
            case ImageType::UNKNOWN:
                printf("Unknown image type\n");
                break;
        }
    }
}

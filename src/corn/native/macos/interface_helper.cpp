#include <bgfx/bgfx.h>
#include <corn/geometry/vec.h>
#include <corn/media/rich_text_frame.h>
#include <corn/util/color.h>
#include "../../render/image_impl.h"
#include "../../render/interface_helper.h"
#include "../../render/polygon_renderer.h"
#include "macos/rich_text_renderer.h"

namespace corn {
    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CSprite& cSprite,
            const Transform2D& cameraTransform) {

        (void)cCamera; (void)cTransform; (void)cSprite; (void)cameraTransform;
        // // Transform
        // Transform2D worldTransform = cameraTransform.inv() * cTransform.getWorldTransform();
        // const Mat3f& mat = worldTransform.getMat();
        // sf::Transform transform {
        //     mat[0][0], mat[0][1], mat[0][2],
        //     mat[1][0], mat[1][1], mat[1][2],
        //     mat[2][0], mat[2][1], mat[2][2],
        // };
        //
        // Vec2f scale = cSprite.image->impl_->scale;
        // if (cSprite.image->impl_->type == ImageType::SVG) {
        //     // SVGs are scaled during rasterization
        //     cSprite.image->impl_->rasterize(Vec2f(1.0f, 1.0f), true);
        //     scale = Vec2f(1.0f, 1.0f);
        // }
        // sf::Sprite& sfSprite = cSprite.image->impl_->sfSprite;
        // sfSprite.setOrigin(sf::Vector2f(0.0f, 0.0f));
        // sfSprite.setPosition(sf::Vector2f(cSprite.location.x, cSprite.location.y));
        // sfSprite.setScale(sf::Vector2f(scale.x, scale.y));
        //
        // cCamera.viewport.impl_->texture.draw(cSprite.image->impl_->sfSprite, transform);
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
        Transform2D worldTransform = worldToCameraTransform * cTransform.getWorldTransform();
        switch (cPolygon.getRenderType()) {
            case CPolygon::RenderType::STATIC:
            case CPolygon::RenderType::DYNAMIC:
                if (cPolygon.thickness > 0) {
                    polygonRenderer->drawEdges(viewID, polygonShader, cPolygon.color, cPolygon.thickness, worldTransform);
                } else {
                    polygonRenderer->drawFill(viewID, polygonShader, cPolygon.color, worldTransform);
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

                TransientPolygonRenderer::draw(viewID, polygonShader, vertices, fillIndices, cPolygon.color, worldTransform);
                break;
            }
        }
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CText& cText,
            const Transform2D& cameraTransform) {

        (void)cCamera; (void)cTransform; (void)cText; (void)cameraTransform;
        // // Draw polygon and fill inside
        // Transform2D worldTransform = cameraTransform.inv() * cTransform.getWorldTransform();
        // const Mat3f& mat = worldTransform.getMat();
        // sf::Transform transform {
        //     mat[0][0], mat[0][1], mat[0][2],
        //     mat[1][0], mat[1][1], mat[1][2],
        //     mat[2][0], mat[2][1], mat[2][2],
        // };
        //
        // Vec2f location(cText.getX(), cText.getY());
        // Vec2f textSize = cText.textRender.getSize();
        //
        // // Render
        // Vec2f seg = location;
        // for (TextRenderImpl::Line& line : cText.textRender.impl_->lines) {
        //     // alignment
        //     switch (cText.textRender.getTextAlign()) {
        //         case TextAlign::LEFT:
        //             seg.x = location.x;
        //             break;
        //         case TextAlign::CENTER:
        //             seg.x = location.x + (textSize.x - line.size.x) / 2;
        //             break;
        //         case TextAlign::RIGHT:
        //             seg.x = location.x + textSize.x - line.size.x;
        //             break;
        //     }
        //
        //     for (const auto& [text, color] : line.contents) {
        //         auto& mutText = const_cast<sf::Text&>(text);
        //         mutText.setOrigin(0.0f, 0.0f);
        //         mutText.setPosition(seg.x, seg.y + cText.textRender.getLinePadding());
        //         const auto [r, g, b, a] = color.getRGBA();  // NOLINT
        //         mutText.setFillColor(sf::Color{ r, g, b, a });
        //
        //         cCamera.viewport.impl_->texture.draw(text, transform);
        //         seg.x += text.getLocalBounds().width;
        //     }
        //     seg.x = location.x;
        //     seg.y += line.size.y;
        // }
    }

    void drawUI(
            bgfx::ViewId viewID,
            UILabel& uiLabel, float maxWidth,
            const Transform2D& transform, const Shader& bitmapShader) {

        RichTextFrame& richTextFrame = uiLabel.getRichTextFrame();
        richTextFrame.setMaxWidth(maxWidth);
        RichTextRenderer* richTextRenderer = richTextFrame.getRichTextRenderer();
        richTextRenderer->setTransform(transform);
        richTextRenderer->draw(viewID, bitmapShader);
    }
}

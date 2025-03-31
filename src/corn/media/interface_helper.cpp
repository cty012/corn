#include <corn/media/text_render.h>
#include <corn/util/color.h>
#include <corn/util/constants.h>
#include "camera_viewport_impl.h"
#include "image_impl.h"
#include "interface_helper.h"
#include "text_render_impl.h"

namespace corn {
    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CSprite& cSprite,
            const Transform2D& cameraTransform) {

        // Transform
        Transform2D worldTransform = cameraTransform.inv() * cTransform.getWorldTransform();
        const Mat3f& mat = worldTransform.getMat();
        sf::Transform transform {
            mat[0][0], mat[0][1], mat[0][2],
            mat[1][0], mat[1][1], mat[1][2],
            mat[2][0], mat[2][1], mat[2][2],
        };

        Vec2f scale = cSprite.image->impl_->scale;
        if (cSprite.image->impl_->type == ImageType::SVG) {
            // SVGs are scaled during rasterization
            cSprite.image->impl_->rasterize(Vec2f(1.0f, 1.0f), true);
            scale = Vec2f(1.0f, 1.0f);
        }
        sf::Sprite& sfSprite = cSprite.image->impl_->sfSprite;
        sfSprite.setOrigin(sf::Vector2f(0.0f, 0.0f));
        sfSprite.setPosition(sf::Vector2f(cSprite.location.x, cSprite.location.y));
        sfSprite.setScale(sf::Vector2f(scale.x, scale.y));

        cCamera.viewport.impl_->texture.draw(cSprite.image->impl_->sfSprite, transform);
    }

    void drawLines(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const std::vector<Vec2f>& vertices, float thickness, const Color& color, bool closed,
            const Transform2D& cameraTransform) {

        // Transform
        Transform2D worldTransform = cameraTransform.inv() * cTransform.getWorldTransform();
        const Mat3f& mat = worldTransform.getMat();
        sf::Transform transform {
            mat[0][0], mat[0][1], mat[0][2],
            mat[1][0], mat[1][1], mat[1][2],
            mat[2][0], mat[2][1], mat[2][2],
        };

        const auto [r, g, b, a] = color.getRGBA();  // NOLINT

        for (size_t i = 0; (closed ? i : i + 1) < vertices.size(); i++) {
            const Vec2f& start = vertices.at(i);
            const Vec2f& end = vertices.at((i + 1) % vertices.size());
            Vec2f diff = end - start;
            float length = diff.norm();
            float angle = std::atan2(diff.y, diff.x) * 180.0f / (float)PI;

            sf::RectangleShape line;
            line.setSize(sf::Vector2f(length, thickness / worldTransform.mapVector(diff.normalize()).norm()));
            line.setOrigin(0, 0);
            line.setPosition(start.x, start.y);
            line.setFillColor(sf::Color{ r, g, b, a });
            line.setRotation(angle);
            cCamera.viewport.impl_->texture.draw(line, transform);
        }
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CLines& cLines,
            const Transform2D& cameraTransform) {

        drawLines(
                cCamera,
                cTransform, cLines.vertices, cLines.thickness, cLines.color, cLines.closed,
                cameraTransform);
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CPolygon& cPolygon,
            const Transform2D& cameraTransform) {

        const Polygon& polygon = cPolygon.polygon;
        if (cPolygon.thickness > 0) {
            // Draw boundary of polygon
            drawLines(
                    cCamera,
                    cTransform, polygon.getVertices(), cPolygon.thickness, cPolygon.color, true,
                    cameraTransform);
            for (const std::vector<Vec2f>& hole: polygon.getHoles()) {
                drawLines(
                        cCamera,
                        cTransform, hole, cPolygon.thickness, cPolygon.color, true,
                        cameraTransform);
            }
        } else {
            // Draw polygon and fill inside
            Transform2D worldTransform = cameraTransform.inv() * cTransform.getWorldTransform();
            const Mat3f& mat = worldTransform.getMat();
            sf::Transform transform {
                mat[0][0], mat[0][1], mat[0][2],
                mat[1][0], mat[1][1], mat[1][2],
                mat[2][0], mat[2][1], mat[2][2],
            };

            const auto [r, g, b, a] = cPolygon.color.getRGBA(); // NOLINT

            const std::vector<std::array<Vec2f, 3>>& triangles = polygon.getTriangles();
            sf::VertexArray varr(sf::Triangles, triangles.size() * 3);
            for (size_t i = 0; i < triangles.size(); i++) {
                for (size_t j = 0; j < 3; j++) {
                    varr[i * 3 + j].position = sf::Vector2f(triangles[i][j].x, triangles[i][j].y);
                    varr[i * 3 + j].color = sf::Color{ r, g, b, a };
                }
            }

            cCamera.viewport.impl_->texture.draw(varr, transform);
        }
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CText& cText,
            const Transform2D& cameraTransform) {

        // Draw polygon and fill inside
        Transform2D worldTransform = cameraTransform.inv() * cTransform.getWorldTransform();
        const Mat3f& mat = worldTransform.getMat();
        sf::Transform transform {
            mat[0][0], mat[0][1], mat[0][2],
            mat[1][0], mat[1][1], mat[1][2],
            mat[2][0], mat[2][1], mat[2][2],
        };

        Vec2f location(cText.getX(), cText.getY());
        Vec2f textSize = cText.textRender.getSize();

        // Render
        Vec2f seg = location;
        for (TextRenderImpl::Line& line : cText.textRender.impl_->lines) {
            // alignment
            switch (cText.textRender.getTextAlign()) {
                case TextAlign::LEFT:
                    seg.x = location.x;
                    break;
                case TextAlign::CENTER:
                    seg.x = location.x + (textSize.x - line.size.x) / 2;
                    break;
                case TextAlign::RIGHT:
                    seg.x = location.x + textSize.x - line.size.x;
                    break;
            }

            for (const auto& [text, color] : line.contents) {
                auto& mutText = const_cast<sf::Text&>(text);
                mutText.setOrigin(0.0f, 0.0f);
                mutText.setPosition(seg.x, seg.y + cText.textRender.getLinePadding());
                const auto [r, g, b, a] = color.getRGBA();  // NOLINT
                mutText.setFillColor(sf::Color{ r, g, b, a });

                cCamera.viewport.impl_->texture.draw(text, transform);
                seg.x += text.getLocalBounds().width;
            }
            seg.x = location.x;
            seg.y += line.size.y;
        }
    }
}

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
            const Vec2f& cameraOffset, const Vec2f&, const sf::Transform& scaleTransform) {

        Transform2D worldTransform = cTransform.getWorldTransform();
        Vec2f worldTranslation = worldTransform.getTranslationComponent();
        Deg worldRotation = worldTransform.getRotationComponent();
        Vec2f anc = worldTranslation - cameraOffset;
        Vec2f loc = cSprite.location;
        Vec2f scale = cSprite.image->impl_->scale;
        if (cSprite.image->impl_->type == ImageType::SVG) {
            // SVGs are scaled during rasterization
            sf::Vector2f newScale = scaleTransform.transformPoint(sf::Vector2f(1.0f, 1.0f));
            sf::Sprite& sfSprite = cSprite.image->impl_->sfSprite;
            cSprite.image->impl_->rasterize(Vec2f(newScale.x, newScale.y), true);
            sfSprite.setOrigin(scaleTransform.transformPoint(sf::Vector2f(-loc.x, -loc.y)));
            sfSprite.setPosition(scaleTransform.transformPoint(sf::Vector2f(anc.x, anc.y)));
            sfSprite.setScale(sf::Vector2f(1.0f, 1.0f));
        } else {
            sf::Vector2f newScale = scaleTransform.transformPoint(sf::Vector2f(scale.x, scale.y));
            sf::Sprite& sfSprite = cSprite.image->impl_->sfSprite;
            sfSprite.setOrigin(scaleTransform.transformPoint(sf::Vector2f(-loc.x, -loc.y)));
            sfSprite.setPosition(scaleTransform.transformPoint(sf::Vector2f(anc.x, anc.y)));
            sfSprite.setScale(newScale);
        }
        cSprite.image->impl_->sfSprite.setRotation(-worldRotation.get());
        cCamera.viewport.impl_->texture.draw(cSprite.image->impl_->sfSprite);
    }

    void drawLines(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const std::vector<Vec2f>& vertices, float thickness, const Color& color, bool closed,
            const Vec2f& cameraOffset, const Vec2f& cameraScale, const sf::Transform& scaleTransform) {

        Transform2D worldTransform = cTransform.getWorldTransform();
        Vec2f worldTranslation = worldTransform.getTranslationComponent();
        Deg worldRotation = worldTransform.getRotationComponent();
        Vec2f anc = worldTranslation - cameraOffset;
        const auto [r, g, b, a] = color.getRGBA();  // NOLINT

        for (size_t i = 0; (closed ? i : i + 1) < vertices.size(); i++) {
            const Vec2f& start = vertices.at(i);
            const Vec2f& end = vertices.at((i + 1) % vertices.size());
            Vec2f diff = end - start;
            float length = diff.norm();
            float angle = std::atan2(diff.y, diff.x) * 180.0f / (float)PI;

            sf::RectangleShape line;
            line.setSize(sf::Vector2f(length, thickness / cameraScale.norm()));
            line.setOrigin(0, 0);
            line.setPosition(anc.x + start.x, anc.y + start.y);
            line.setFillColor(sf::Color{ r, g, b, a });
            line.setRotation(-worldRotation.get() + angle);
            cCamera.viewport.impl_->texture.draw(line, scaleTransform);
        }
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CLines& cLines,
            const Vec2f& cameraOffset, const Vec2f& cameraScale, const sf::Transform& scaleTransform) {

        drawLines(
                cCamera,
                cTransform, cLines.vertices, cLines.thickness, cLines.color, cLines.closed,
                cameraOffset, cameraScale, scaleTransform);
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CPolygon& cPolygon,
            const Vec2f& cameraOffset, const Vec2f& cameraScale, const sf::Transform& scaleTransform) {

        const Polygon& polygon = cPolygon.polygon;
        if (cPolygon.thickness > 0) {
            // Draw boundary of polygon
            drawLines(
                    cCamera,
                    cTransform, polygon.getVertices(), cPolygon.thickness, cPolygon.color, true,
                    cameraOffset, cameraScale, scaleTransform);
            for (const std::vector<Vec2f>& hole: polygon.getHoles()) {
                drawLines(
                        cCamera,
                        cTransform, hole, cPolygon.thickness, cPolygon.color, true,
                        cameraOffset, cameraScale, scaleTransform);
            }
        } else {
            // Draw polygon and fill inside
            Transform2D worldTransform = cTransform.getWorldTransform();
            Vec2f worldTranslation = worldTransform.getTranslationComponent();
            Deg worldRotation = worldTransform.getRotationComponent();
            Vec2f anc = worldTranslation - cameraOffset;
            const auto [r, g, b, a] = cPolygon.color.getRGBA(); // NOLINT

            sf::Transform rotateTransform;
            rotateTransform.rotate(-worldRotation.get());

            const std::vector<std::array<Vec2f, 3>>& triangles = polygon.getTriangles();
            sf::VertexArray varr(sf::Triangles, triangles.size() * 3);
            for (size_t i = 0; i < triangles.size(); i++) {
                for (size_t j = 0; j < 3; j++) {
                    varr[i * 3 + j].position =
                            sf::Vector2f(anc.x, anc.y) +
                            rotateTransform.transformPoint(sf::Vector2f(triangles[i][j].x, triangles[i][j].y));
                    varr[i * 3 + j].color = sf::Color{ r, g, b, a };
                }
            }

            cCamera.viewport.impl_->texture.draw(varr, scaleTransform);
        }
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CText& cText,
            const Vec2f& cameraOffset, const Vec2f& cameraScale, const sf::Transform& scaleTransform) {

        (void)cameraScale;

        Transform2D worldTransform = cTransform.getWorldTransform();
        Vec2f worldTranslation = worldTransform.getTranslationComponent();
        Deg worldRotation = worldTransform.getRotationComponent();
        Vec2f anc = worldTranslation - cameraOffset;
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
                Vec2f loc(seg.x, seg.y + cText.textRender.getLinePadding());

                mutText.setOrigin(-loc.x, -loc.y);
                mutText.setPosition(anc.x, anc.y);
                mutText.setRotation(-worldRotation.get());
                const auto [r, g, b, a] = color.getRGBA();  // NOLINT
                mutText.setFillColor(sf::Color{ r, g, b, a });

                cCamera.viewport.impl_->texture.draw(text, scaleTransform);
                seg.x += text.getLocalBounds().width;
            }
            seg.x = location.x;
            seg.y += line.size.y;
        }
    }
}

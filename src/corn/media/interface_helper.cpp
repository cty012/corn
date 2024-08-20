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
            const Vec2& cameraOffset, const Vec2&, const sf::Transform& scaleTransform) {

        auto [worldLocation, worldRotation] = cTransform.getWorldTransform();
        auto [ancX, ancY] = worldLocation - cameraOffset;
        auto [locX, locY] = cSprite.location;
        auto [scaleX, scaleY] = cSprite.image->impl_->scale;
        sf::Sprite& sfSprite = cSprite.image->impl_->sfSprite;
        sfSprite.setOrigin(-locX, -locY);
        sfSprite.setPosition(ancX, ancY);
        sfSprite.setScale(scaleX, scaleY);
        sfSprite.setRotation(-worldRotation.get());
        cCamera.viewport.impl_->texture.draw(sfSprite, scaleTransform);
    }

    void drawLines(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const std::vector<Vec2>& vertices, float thickness, const Color& color, bool closed,
            const Vec2& cameraOffset, const Vec2& cameraScale, const sf::Transform& scaleTransform) {

        auto [worldLocation, worldRotation] = cTransform.getWorldTransform();
        auto [ancX, ancY] = worldLocation - cameraOffset;
        auto [r, g, b, a] = color.getRGBA();

        for (size_t i = 0; (closed ? i : i + 1) < vertices.size(); i++) {
            auto [startX, startY] = vertices.at(i);
            auto [endX, endY] = vertices.at((i + 1) % vertices.size());
            float diffX = endX - startX, diffY = endY - startY;
            float length = std::sqrt(diffX * diffX + diffY * diffY);
            float angle = std::atan2(diffY, diffX) * 180.0f / (float)PI;

            sf::RectangleShape line;
            line.setSize(sf::Vector2f(length, thickness / cameraScale.norm()));
            line.setOrigin(0, 0);
            line.setPosition(ancX + startX, ancY + startY);
            line.setFillColor(sf::Color{ r, g, b, a });
            line.setRotation(-worldRotation.get() + angle);
            cCamera.viewport.impl_->texture.draw(line, scaleTransform);
        }
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CLines& cLines,
            const Vec2& cameraOffset, const Vec2& cameraScale, const sf::Transform& scaleTransform) {

        drawLines(
                cCamera,
                cTransform, cLines.vertices, cLines.thickness, cLines.color, cLines.closed,
                cameraOffset, cameraScale, scaleTransform);
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CPolygon& cPolygon,
            const Vec2& cameraOffset, const Vec2& cameraScale, const sf::Transform& scaleTransform) {

        const Polygon& polygon = cPolygon.polygon;
        if (cPolygon.thickness > 0) {
            // Draw boundary of polygon
            drawLines(
                    cCamera,
                    cTransform, polygon.getVertices(), cPolygon.thickness, cPolygon.color, true,
                    cameraOffset, cameraScale, scaleTransform);
            for (const std::vector<Vec2>& hole: polygon.getHoles()) {
                drawLines(
                        cCamera,
                        cTransform, hole, cPolygon.thickness, cPolygon.color, true,
                        cameraOffset, cameraScale, scaleTransform);
            }
        } else {
            // Draw polygon and fill inside
            auto [worldLocation, worldRotation] = cTransform.getWorldTransform();
            auto [ancX, ancY] = worldLocation - cameraOffset;
            auto [r, g, b, a] = cPolygon.color.getRGBA();

            sf::Transform rotateTransform;
            rotateTransform.rotate(-worldRotation.get());

            const std::vector<std::array<Vec2, 3>>& triangles = polygon.getTriangles();
            sf::VertexArray varr(sf::Triangles, triangles.size() * 3);
            for (size_t i = 0; i < triangles.size(); i++) {
                for (size_t j = 0; j < 3; j++) {
                    varr[i * 3 + j].position =
                            sf::Vector2f(ancX, ancY) +
                            rotateTransform.transformPoint(sf::Vector2f(triangles[i][j].x, triangles[i][j].y));
                    varr[i * 3 + j].color = sf::Color{r, g, b, a};
                }
            }

            cCamera.viewport.impl_->texture.draw(varr, scaleTransform);
        }
    }

    void draw(
            const CCamera& cCamera,
            const CTransform2D& cTransform, const CText& cText,
            const Vec2& cameraOffset, const Vec2&, const sf::Transform& scaleTransform) {

        auto [worldLocation, worldRotation] = cTransform.getWorldTransform();
        auto [ancX, ancY] = worldLocation - cameraOffset;
        Vec2 location(cText.getX(), cText.getY());

        // Render
        float segX = location.x, segY = location.y;
        for (TextRenderImpl::Line& line : cText.textRender.impl_->lines) {
            for (const auto& [text, color] : line.contents) {
                auto& mutText = const_cast<sf::Text&>(text);
                float locX = segX;
                float locY = segY + cText.textRender.getLinePadding();

                mutText.setOrigin(-locX, -locY);
                mutText.setPosition(ancX, ancY);
                mutText.setRotation(-worldRotation.get());
                auto [r, g, b, a] = color.getRGBA();
                mutText.setFillColor(sf::Color(r, g, b, a));

                cCamera.viewport.impl_->texture.draw(text, scaleTransform);
                segX += text.getLocalBounds().width;
            }
            segX = location.x;
            segY += line.size.y;
        }
    }
}

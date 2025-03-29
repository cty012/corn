#include <array>
#include <boost/geometry.hpp>
#include <mapbox/earcut.hpp>
#include <corn/geometry/deg.h>
#include <corn/geometry/operations.h>
#include <corn/geometry/polygon.h>
#include "polygon_helper.h"

namespace corn {
    namespace bg = boost::geometry;
    using point_t = bg::model::d2::point_xy<float>;
    using polygon_t = bg::model::polygon<point_t>;
    using ring_t = bg::model::ring<point_t>;

    Polygon::Polygon() noexcept : type_(PolygonType::EMPTY) {
        this->typeDirty_ = false;
        this->centroidAndAreaDirty_ = false;
        this->bboxDirty_ = false;
        this->trianglesDirty_ = false;
        this->area_ = 0.0f;
    }

    Polygon::Polygon(const std::vector<Vec2f>& vertices) noexcept : vertices_(vertices), type_(PolygonType::EMPTY) {
        this->typeDirty_ = true;
        this->centroidAndAreaDirty_ = true;
        this->bboxDirty_ = true;
        this->trianglesDirty_ = true;
        this->area_ = 0.0f;
    }

    Polygon::Polygon(const std::vector<Vec2f>& vertices, const std::vector<std::vector<Vec2f>>& holes) noexcept
            : vertices_(vertices), holes_(holes), type_(PolygonType::EMPTY) {

        this->typeDirty_ = true;
        this->centroidAndAreaDirty_ = true;
        this->bboxDirty_ = true;
        this->trianglesDirty_ = true;
        this->area_ = 0.0f;
    }

    Polygon Polygon::createTriangle(const corn::Vec2f& v1, const corn::Vec2f& v2, const corn::Vec2f& v3) noexcept {
        return Polygon({ v1, v2, v3 });
    }

    Polygon Polygon::createRectangle(const Vec2f& topLeft, float width, float height) noexcept {
        return Polygon({
            topLeft,
            topLeft + Vec2f(width, 0),
            topLeft + Vec2f(width, height),
            topLeft + Vec2f(0, height)
        });
    }

    Polygon Polygon::createCircle(const Vec2f& center, float radius, size_t segments) noexcept {
        std::vector<Vec2f> vertices;
        vertices.reserve(segments);
        for (size_t i = 0; i < segments; i++) {
            Deg angle = 360.0f * (float)i / (float)segments;
            vertices.push_back(center + radius * Vec2f(angle.cos(), angle.sin()));
        }
        return Polygon(vertices);
    }

    const std::vector<Vec2f>& Polygon::getVertices() const noexcept {
        return this->vertices_;
    }

    const std::vector<std::vector<Vec2f>>& Polygon::getHoles() const noexcept {
        return this->holes_;
    }

    void Polygon::setVertices(const std::vector<Vec2f>& vertices, const std::vector<std::vector<Vec2f>>& holes) {
        this->vertices_ = vertices;
        this->holes_ = holes;
        this->typeDirty_ = true;
        this->centroidAndAreaDirty_ = true;
        this->bboxDirty_ = true;
        this->trianglesDirty_ = true;
        this->area_ = 0.0f;
    }

    bool Polygon::contains(const corn::Vec2f& point, bool edgeInclusive) const {
        if (this->vertices_.empty()) {
            return false;
        }

        // First check if it is in the bounding box
        const auto& [tl, br] = this->getBBox();
        if (tl.x > point.x || tl.y > point.y || br.x < point.x || br.y < point.y) {
            return false;
        }

        // If so, first check if it is on the boundary
        std::function<bool(const Vec2f&, const std::vector<Vec2f>&)> pointIsOnRing =
                [](const Vec2f& point, const std::vector<Vec2f>& ring) {
                    if (ring.size() < 2) return false;
                    for (size_t i = 0; i < ring.size(); i++) {
                        float x1 = ring[i].x;
                        float y1 = ring[i].y;
                        float x2 = ring[(i + 1) % ring.size()].x;
                        float y2 = ring[(i + 1) % ring.size()].y;
                        // Linearity check
                        bool isCollinear = x1 * (y2 - point.y) + x2 * (point.y - y1) + point.x * (y1 - y2) == 0;
                        // Between check
                        bool xIsBetween = (std::min(x1, x2) <= point.x) && (point.x <= std::max(x1, x2));
                        bool yIsBetween = (std::min(y1, y2) <= point.y) && (point.y <= std::max(y1, y2));
                        if (isCollinear && xIsBetween && yIsBetween) {
                            return true;
                        }
                    }
                    return false;
                };
        if (pointIsOnRing(point, this->vertices_)) {
            return edgeInclusive;
        }
        for (const std::vector<Vec2f>& ring : this->holes_) {
            if (pointIsOnRing(point, ring)) {
                return edgeInclusive;
            }
        }

        // Then check if it is inside/on the polygon
        const std::vector<std::array<Vec2f, 3>>& triangles = this->getTriangles();
        return std::any_of(
                triangles.begin(), triangles.end(),
                [&point](const std::array<Vec2f, 3>& triangle) {
                    const auto& [v1, v2, v3] = triangle;
                    float c1 = cross(v2 - v1, point - v1);
                    float c2 = cross(v3 - v2, point - v2);
                    float c3 = cross(v1 - v3, point - v3);
                    return (c1 >= 0 && c2 >= 0 && c3 >= 0) || (c1 <= 0 && c2 <= 0 && c3 <= 0);
                });
    }

    void Polygon::translate(const Vec2f& displacement) noexcept {
        // Translate the vertices
        for (Vec2f& vertex : this->vertices_) {
            vertex += displacement;
        }
        for (std::vector<Vec2f>& hole : this->holes_) {
            for (Vec2f& vertex : hole) {
                vertex += displacement;
            }
        }

        // Translate the centroid and bounding box
        this->centroid_ += displacement;
        this->bbox_.first += displacement;
        this->bbox_.second += displacement;

        // Translate the triangles
        if (!this->trianglesDirty_) {
            for (std::array<Vec2f, 3>& triangle: this->triangles_) {
                for (Vec2f& vertex: triangle) {
                    vertex += displacement;
                }
            }
        }
    }

    PolygonType Polygon::getType() const {
        if (this->typeDirty_) {
            this->calcType();
        }
        return this->type_;
    }

    const Vec2f& Polygon::getCentroid() const {
        if (this->centroidAndAreaDirty_) {
            this->calcCentroidAndArea();
        }
        return this->centroid_;
    }

    float Polygon::getArea() const {
        if (this->centroidAndAreaDirty_) {
            this->calcCentroidAndArea();
        }
        return this->area_;
    }

    const std::pair<Vec2f, Vec2f>& Polygon::getBBox() const {
        if (this->bboxDirty_) {
            this->calcBBox();
        }
        return this->bbox_;
    }

    const std::vector<std::array<Vec2f, 3>>& Polygon::getTriangles() const {
        if (this->trianglesDirty_) {
            this->triangulate();
        }
        return this->triangles_;
    }

    void Polygon::calcType() const {
        polygon_t polygon;
        toBoostPolygon(polygon, this->vertices_, this->holes_);

        // Categorize the polygon
        if (bg::is_empty(polygon)) {
            this->type_ = PolygonType::EMPTY;
        } else if (!bg::is_valid(polygon)) {
            this->type_ = PolygonType::INVALID;
        } else if (!polygon.inners().empty()) {
            this->type_ = PolygonType::COMPLEX;
        } else if (bg::is_convex(polygon)) {
            this->type_ = PolygonType::CONVEX;
        } else {
            this->type_ = PolygonType::CONCAVE;
        }

        this->typeDirty_ = false;
    }

    void Polygon::calcCentroidAndArea() const {
        // Skip if the polygon is invalid
        if (this->getType() == PolygonType::INVALID) {
            this->centroidAndAreaDirty_ = false;
            return;
        }

        // Reset
        this->area_ = 0.0f;
        this->centroid_ = Vec2f::O();

        // Skip if the polygon is empty
        if (this->getType() == PolygonType::EMPTY) {
            this->centroidAndAreaDirty_ = false;
            return;
        }

        // Calculate the centroid of each triangle and scale them by the area
        for (const auto& [v1, v2, v3] : this->getTriangles()) {
            float area = triangleArea(v1, v2, v3);
            this->area_ += area;
            this->centroid_ += triangleCentroid(v1, v2, v3) * area;
        }
        this->centroid_ *= (1 / this->area_);

        this->centroidAndAreaDirty_ = false;
    }

    void Polygon::calcBBox() const {
        // Skip if the polygon is invalid/empty
        if (this->getType() == PolygonType::INVALID || this->getType() == PolygonType::EMPTY) {
            this->bbox_ = { Vec2f::O(), Vec2f::O() };
            this->bboxDirty_ = false;
            return;
        }

        // Reset
        this->bbox_ = {
                Vec2f(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()),
                Vec2f(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity())
        };

        // Calculate the bounding box
        for (const std::array<Vec2f, 3>& triangle : this->getTriangles()) {
            for (const Vec2f& v : triangle) {
                this->bbox_.first = min(this->bbox_.first, v);
                this->bbox_.second = max(this->bbox_.second, v);
            }
        }

        this->bboxDirty_ = false;
    }

    void Polygon::triangulate() const {
        this->triangles_.clear();

        // Skip if the polygon is invalid
        if (this->getType() == PolygonType::INVALID) {
            this->trianglesDirty_ = false;
            return;
        }

        std::vector<std::vector<std::array<float, 2>>> polygonToCut;
        std::vector<std::array<float, 2>> flattenedPolygon;
        toEarcutPolygon(polygonToCut, flattenedPolygon, this->vertices_, this->holes_);

        // Triangulate
        std::vector<size_t> indices = mapbox::earcut<size_t>(polygonToCut);
        if (indices.size() % 3 != 0) {
            throw std::logic_error("Invalid triangulation result");
        }

        this->triangles_.reserve(indices.size() / 3);
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            size_t i1 = indices[i], i2 = indices[i+1], i3 = indices[i+2];
            this->triangles_.push_back({
                Vec2f(flattenedPolygon[i1][0], flattenedPolygon[i1][1]),
                Vec2f(flattenedPolygon[i2][0], flattenedPolygon[i2][1]),
                Vec2f(flattenedPolygon[i3][0], flattenedPolygon[i3][1])
            });
        }

        this->trianglesDirty_ = false;
    }
}

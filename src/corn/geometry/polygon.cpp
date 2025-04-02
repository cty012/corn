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
        this->triangleIndicesDirty_ = false;
        this->area_ = 0.0f;
    }

    Polygon::Polygon(const std::vector<Vec2f>& vertices) noexcept : vertices_(vertices), type_(PolygonType::EMPTY) {
        this->typeDirty_ = true;
        this->centroidAndAreaDirty_ = true;
        this->bboxDirty_ = true;
        this->triangleIndicesDirty_ = true;
        this->area_ = 0.0f;

        // Calculate the flattened vertices
        this->verticesFlat_.clear();
        this->verticesFlat_.reserve(this->vertices_.size());
        this->verticesFlat_.insert(this->verticesFlat_.end(), this->vertices_.begin(), this->vertices_.end());
    }

    Polygon::Polygon(const std::vector<Vec2f>& vertices, const std::vector<std::vector<Vec2f>>& holes) noexcept
            : vertices_(vertices), holes_(holes), type_(PolygonType::EMPTY) {

        this->typeDirty_ = true;
        this->centroidAndAreaDirty_ = true;
        this->bboxDirty_ = true;
        this->triangleIndicesDirty_ = true;
        this->area_ = 0.0f;

        // Calculate the flattened vertices
        this->verticesFlat_.clear();
        this->verticesFlat_.reserve(vertices.size() + holes.size());
        for (size_t i = 0; i < holes.size() + 1; i++) {
            const std::vector<Vec2f>& ring = (i == 0) ? this->vertices_ : this->holes_[i - 1];
            this->verticesFlat_.insert(this->verticesFlat_.end(), ring.begin(), ring.end());
        }
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

    const std::vector<Vec2f>& Polygon::getVerticesFlat() const noexcept {
        return this->verticesFlat_;
    }

    void Polygon::setVertices(std::vector<Vec2f> vertices, std::vector<std::vector<Vec2f>> holes) {
        this->vertices_ = std::move(vertices);
        this->holes_ = std::move(holes);
        this->typeDirty_ = true;
        this->centroidAndAreaDirty_ = true;
        this->bboxDirty_ = true;
        this->triangleIndicesDirty_ = true;
        this->area_ = 0.0f;

        // Calculate the flattened vertices
        this->verticesFlat_.clear();
        this->verticesFlat_.reserve(vertices.size() + holes.size());
        this->verticesFlat_.insert(this->verticesFlat_.end(), vertices.begin(), vertices.end());
        for (const std::vector<Vec2f>& hole : holes) {
            this->verticesFlat_.insert(this->verticesFlat_.end(), hole.begin(), hole.end());
        }
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
        for (size_t i = 0; i < this->holes_.size(); i++) {
            const std::vector<Vec2f>& ring = (i == 0) ? this->vertices_ : this->holes_[i - 1];
            if (pointIsOnRing(point, ring)) {
                return edgeInclusive;
            }
        }

        // Then check if it is inside/on the polygon
        (void)this->getTriangleIndices();
        for (size_t i = 0; i + 2 < this->triangleIndices_.size(); i += 3) {
            const Vec2f& v1 = this->verticesFlat_[this->triangleIndices_[i]];
            const Vec2f& v2 = this->verticesFlat_[this->triangleIndices_[i + 1]];
            const Vec2f& v3 = this->verticesFlat_[this->triangleIndices_[i + 2]];
            float c1 = cross(v2 - v1, point - v1);
            float c2 = cross(v3 - v2, point - v2);
            float c3 = cross(v1 - v3, point - v3);
            if ((c1 >= 0 && c2 >= 0 && c3 >= 0) || (c1 <= 0 && c2 <= 0 && c3 <= 0)) {
                return true;
            }
        }
        return false;
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

    const std::vector<size_t>& Polygon::getTriangleIndices() const {
        if (this->triangleIndicesDirty_) {
            this->triangulate();
        }
        return this->triangleIndices_;
    }

    std::vector<std::array<Vec2f, 3>> Polygon::getTriangles() const {
        std::vector<std::array<Vec2f, 3>> triangles;
        for (size_t i = 0; i < this->getTriangleIndices().size(); i++) {
            const Vec2f& v1 = this->verticesFlat_[this->triangleIndices_[i]];
            const Vec2f& v2 = this->verticesFlat_[this->triangleIndices_[(i + 1) % this->triangleIndices_.size()]];
            const Vec2f& v3 = this->verticesFlat_[this->triangleIndices_[(i + 2) % this->triangleIndices_.size()]];
            triangles[i] = { v1, v2, v3 };
        }
        return triangles;
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
        (void)this->getTriangleIndices();
        for (size_t i = 0; i + 2 < this->triangleIndices_.size(); i++) {
            const Vec2f& v1 = this->verticesFlat_[this->triangleIndices_[i]];
            const Vec2f& v2 = this->verticesFlat_[this->triangleIndices_[i + 1]];
            const Vec2f& v3 = this->verticesFlat_[this->triangleIndices_[i + 2]];
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
        for (size_t i = 0; i < this->getTriangleIndices().size(); i++) {
            this->bbox_.first = min(this->bbox_.first, this->verticesFlat_[i]);
            this->bbox_.second = max(this->bbox_.second, this->verticesFlat_[i]);
        }

        this->bboxDirty_ = false;
    }

    void Polygon::triangulate() const {
        this->triangleIndices_.clear();

        // Skip if the polygon is invalid
        if (this->getType() == PolygonType::INVALID) {
            this->triangleIndicesDirty_ = false;
            return;
        }

        std::vector<std::vector<std::array<float, 2>>> polygonToCut;
        std::vector<std::array<float, 2>> flattenedPolygon;
        toEarcutPolygon(polygonToCut, flattenedPolygon, this->vertices_, this->holes_);

        // Triangulate
        this->triangleIndices_ = mapbox::earcut<size_t>(polygonToCut);
        if (this->triangleIndices_.size() % 3 != 0) {
            throw std::logic_error("Invalid triangulation result");
        }

        this->triangleIndicesDirty_ = false;
    }
}

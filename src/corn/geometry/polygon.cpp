#include <array>
#include <boost/geometry.hpp>
#include <mapbox/earcut.hpp>
#include <corn/geometry/polygon.h>
#include "polygon_helper.h"

namespace corn {
    namespace bg = boost::geometry;
    using point_t = bg::model::d2::point_xy<float>;
    using polygon_t = bg::model::polygon<point_t>;
    using ring_t = bg::model::ring<point_t>;

    Polygon::Polygon() : type_(PolygonType::EMPTY) {}

    Polygon::Polygon(const std::vector<Vec2>& vertices) : type_(PolygonType::EMPTY), vertices_(vertices) {
        triangulate();
    }

    Polygon::Polygon(const std::vector<Vec2>& vertices, const std::vector<std::vector<Vec2>>& holes)
            : type_(PolygonType::EMPTY), vertices_(vertices), holes_(holes) {

        triangulate();
    }

    const std::vector<Vec2>& Polygon::getVertices() const {
        return this->vertices_;
    }

    const std::vector<std::vector<Vec2>>& Polygon::getHoles() const {
        return this->holes_;
    }

    void Polygon::setVertices(const std::vector<Vec2>& vertices, const std::vector<std::vector<Vec2>>& holes) {
        this->vertices_ = vertices;
        this->holes_ = holes;
        triangulate();
    }

    void Polygon::triangulate() {
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

        // Triangulate the polygon
        this->triangles_.clear();
        if (this->type_ != PolygonType::INVALID) {
            std::vector<std::vector<std::array<float, 2>>> polygonToCut;
            std::vector<std::array<float, 2>> flattenedPolygon;
            toEarcutPolygon(polygonToCut, flattenedPolygon, this->vertices_, this->holes_);

            // Triangulate
            std::vector<size_t> indices = mapbox::earcut<size_t>(polygonToCut);
            this->triangles_.reserve(indices.size() / 3);
            for (size_t i = 0; i + 2 < indices.size(); i += 3) {
                size_t i1 = indices[i], i2 = indices[i+1], i3 = indices[i+2];
                this->triangles_.push_back({
                    Vec2(flattenedPolygon[i1][0], flattenedPolygon[i1][1]),
                    Vec2(flattenedPolygon[i2][0], flattenedPolygon[i2][1]),
                    Vec2(flattenedPolygon[i3][0], flattenedPolygon[i3][1])
                });
            }
        }
    }
}

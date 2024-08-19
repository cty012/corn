#include "polygon_helper.h"

namespace corn {
    namespace bg = boost::geometry;
    using point_t = bg::model::d2::point_xy<float>;
    using polygon_t = bg::model::polygon<point_t>;
    using ring_t = bg::model::ring<point_t>;

    void toBoostPolygon(
            polygon_t& polygon,
            const std::vector<Vec2>& vertices, const std::vector<std::vector<Vec2>>& holes) {

        bg::clear(polygon);

        // Outer ring
        for (const Vec2& vertex : vertices) {
            bg::append(polygon.outer(), point_t(vertex.x, vertex.y));
        }

        // Holes
        for (const std::vector<Vec2>& hole : holes) {
            ring_t holeRing;
            for (const Vec2& vertex : hole) {
                bg::append(holeRing, point_t(vertex.x, vertex.y));
            }
            polygon.inners().push_back(holeRing);
        }

        // Orient and close the rings
        bg::correct(polygon);
    }

    void toEarcutPolygon(
            std::vector<std::vector<std::array<float, 2>>>& polygon, std::vector<std::array<float, 2>>& flattenedPolygon,
            const std::vector<Vec2>& vertices, const std::vector<std::vector<Vec2>>& holes) {

        polygon.clear();
        flattenedPolygon.clear();
        polygon.resize(1 + holes.size());

        // Add vertices and holes
        polygon[0].reserve(vertices.size());
        for (const auto& vertex : vertices) {
            polygon[0].push_back({vertex.x, vertex.y});
            flattenedPolygon.push_back({vertex.x, vertex.y});
        }
        for (size_t i = 0; i < holes.size(); i++) {
            polygon[i+1].reserve(holes[i].size());
            for (const auto& vertex : holes[i]) {
                polygon[i+1].push_back({vertex.x, vertex.y});
                flattenedPolygon.push_back({vertex.x, vertex.y});
            }
        }
    }

    void fromBoostPolygon(
            std::vector<Vec2>& vertices, std::vector<std::vector<Vec2>>& holes,
            const polygon_t& polygon) {

        vertices.clear();
        holes.clear();

        // Add vertices and holes
        vertices.reserve(polygon.outer().size());
        for (const auto& point : polygon.outer()) {
            vertices.emplace_back(point.x(), point.y());
        }
        vertices.pop_back();  // Remove the last point, which is the same as the first

        for (const auto& hole : polygon.inners()) {
            holes.emplace_back();
            holes.back().reserve(hole.size());
            for (const auto& point : hole) {
                holes.back().emplace_back(point.x(), point.y());
            }
            holes.back().pop_back();  // Remove the last point, which is the same as the first
        }
    }

    void fromEarcutPolygon(
            std::vector<Vec2>& vertices, std::vector<std::vector<Vec2>>& holes,
            const std::vector<std::vector<std::array<float, 2>>>& polygon) {

        vertices.clear();
        holes.clear();

        // Add vertices and holes
        vertices.reserve(polygon[0].size());
        for (const auto& vertex : polygon[0]) {
            vertices.emplace_back(vertex[0], vertex[1]);
        }
        for (size_t i = 1; i < polygon.size(); i++) {
            holes.emplace_back();
            holes.back().reserve(polygon[i].size());
            for (const auto& vertex : polygon[i]) {
                holes.back().emplace_back(vertex[0], vertex[1]);
            }
        }
    }
}

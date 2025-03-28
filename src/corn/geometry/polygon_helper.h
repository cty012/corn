#pragma once

#include <array>
#include <vector>
#include <boost/geometry.hpp>
#include <corn/geometry/vec.h>

namespace corn {
    /**
     * @brief Convert a polygon to a Boost polygon.
     * @param polygon The Boost polygon.
     * @param vertices The vertices of the polygon.
     * @param holes The holes of the polygon.
     */
    void toBoostPolygon(
            boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<float>>& polygon,
            const std::vector<Vec2f>& vertices, const std::vector<std::vector<Vec2f>>& holes);

    /**
     * @brief Convert a Boost polygon to a polygon.
     * @param vertices The vertices of the polygon.
     * @param holes The holes of the polygon.
     * @param polygon The Boost polygon.
     */
    void fromBoostPolygon(
            std::vector<Vec2f>& vertices, std::vector<std::vector<Vec2f>>& holes,
            const boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<float>>& polygon);

    /**
     * @brief Convert a polygon to an Earcut polygon.
     * @param polygon The Earcut polygon.
     * @param flattenedPolygon The flattened Earcut polygon.
     * @param vertices The vertices of the polygon.
     * @param holes The holes of the polygon.
     */
    void toEarcutPolygon(
            std::vector<std::vector<std::array<float, 2>>>& polygon, std::vector<std::array<float, 2>>& flattenedPolygon,
            const std::vector<Vec2f>& vertices, const std::vector<std::vector<Vec2f>>& holes);

    /**
     * @brief Convert an Earcut polygon to a polygon.
     * @param vertices The vertices of the polygon.
     * @param holes The holes of the polygon.
     * @param polygon The Earcut polygon.
     */
    void fromEarcutPolygon(
            std::vector<Vec2f>& vertices, std::vector<std::vector<Vec2f>>& holes,
            const std::vector<std::vector<std::array<float, 2>>>& polygon);
}

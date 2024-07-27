#include <cmath>
#include <format>
#include <boost/geometry.hpp>
#include <corn/geometry/operations.h>
#include <corn/geometry/polygon.h>
#include <corn/geometry/rotation.h>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>
#include "polygon_helper.h"

namespace corn {
    namespace bg = boost::geometry;
    using point_t = bg::model::d2::point_xy<float>;
    using polygon_t = bg::model::polygon<point_t>;
    using ring_t = bg::model::ring<point_t>;

    std::string toString(const Vec2& vec) {
        return std::format("<{}, {}>", vec.x, vec.y);
    }

    std::string toString(const Vec3& vec) {
        return std::format("<{}, {}, {}>", vec.x, vec.y, vec.z);
    }

    std::string toString(const Vec4& vec) {
        return std::format("<{}, {}, {}, {}>", vec.x, vec.y, vec.z, vec.w);
    }

    Vec2 min(const Vec2& v1, const Vec2& v2) {
        return { std::min(v1.x, v2.x), std::min(v1.y, v2.y) };
    }

    Vec2 max(const Vec2& v1, const Vec2& v2) {
        return { std::max(v1.x, v2.x), std::max(v1.y, v2.y) };
    }

    Vec3 min(const Vec3& v1, const Vec3& v2) {
        return { std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z) };
    }

    Vec3 max(const Vec3& v1, const Vec3& v2) {
        return { std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z) };
    }

    Vec4 min(const Vec4& v1, const Vec4& v2) {
        return { std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z), std::min(v1.w, v2.w) };
    }

    Vec4 max(const Vec4& v1, const Vec4& v2) {
        return { std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z), std::max(v1.w, v2.w) };
    }

    Vec2 clamp(const Vec2& minimum, const Vec2& value, const Vec2& maximum) {
        return {
            clamp(minimum.x, value.x, maximum.x),
            clamp(minimum.y, value.y, maximum.y),
        };
    }

    Vec3 clamp(const Vec3& minimum, const Vec3& value, const Vec3& maximum) {
        return {
            clamp(minimum.x, value.x, maximum.x),
            clamp(minimum.y, value.y, maximum.y),
            clamp(minimum.z, value.z, maximum.z),
        };
    }

    Vec4 clamp(const Vec4& minimum, const Vec4& value, const Vec4& maximum) {
        return {
            clamp(minimum.x, value.x, maximum.x),
            clamp(minimum.y, value.y, maximum.y),
            clamp(minimum.z, value.z, maximum.z),
            clamp(minimum.w, value.w, maximum.w),
        };
    }

    float dot(const Vec2& lhs, const Vec2& rhs) noexcept {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    float dot(const Vec3& lhs, const Vec3& rhs) noexcept {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    float dot(const Vec4& lhs, const Vec4& rhs) noexcept {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
    }

    float cross(const Vec2& v1, const Vec2& v2) noexcept {
        return v1.x * v2.y - v1.y * v2.x;
    }

    Vec3 cross(const Vec3& v1, const Vec3& v2) noexcept {
        return {
            v1.y * v2.z - v2.y * v1.z,
            v1.z * v2.x - v2.z * v1.x,
            v1.x * v2.y - v2.x * v1.y,
        };
    }

    Vec2 rotate(const Vec2& point, const Deg& deg) noexcept {
        float cdeg = deg.cos();
        float sdeg = deg.sin();
        return { point.x * cdeg + point.y * sdeg, -point.x * sdeg + point.y * cdeg };
    }

    Vec3 rotate(const Vec3& point, const Quaternion& quaternion) noexcept {
        Quaternion p = { 0, point.x, point.y, point.z };
        Vec4 result = (quaternion * p * quaternion.inv()).val;
        return { result.y, result.z, result.w };  // NOLINT
    }

    float triangleArea(const Vec2& v1, const Vec2& v2, const Vec2& v3) noexcept {
        return 0.5f * std::abs(v1.x * (v2.y - v3.y) + v2.x * (v3.y - v1.y) + v3.x * (v1.y - v2.y));
    }

    float triangleArea(const Vec3& v1, const Vec3& v2, const Vec3& v3) noexcept {
        Vec3 ab = v2 - v1;
        Vec3 ac = v3 - v1;
        Vec3 crossProduct = cross(ab, ac);
        return 0.5f * crossProduct.norm();
    }

    Vec2 triangleCentroid(const Vec2& v1, const Vec2& v2, const Vec2& v3) noexcept {
        return (v1 + v2 + v3) * (1.0f / 3.0f);
    }

    Vec3 triangleCentroid(const Vec3& v1, const Vec3& v2, const Vec3& v3) noexcept {
        return (v1 + v2 + v3) * (1.0f / 3.0f);
    }

    std::vector<Polygon> polygonUnion(const Polygon& polygon1, const Polygon& polygon2) noexcept {
        std::vector<Polygon> result;
        polygon_t bg_polygon1, bg_polygon2;
        std::vector<polygon_t> bg_result;

        // Convert to Boost polygons
        toBoostPolygon(bg_polygon1, polygon1.getVertices(), polygon1.getHoles());
        toBoostPolygon(bg_polygon2, polygon2.getVertices(), polygon2.getHoles());

        // Union
        bg::union_(bg_polygon1, bg_polygon2, bg_result);

        // Convert back to Polygon
        for (const auto& bg_polygon : bg_result) {
            std::vector<Vec2> vertices;
            std::vector<std::vector<Vec2>> holes;
            fromBoostPolygon(vertices, holes, bg_polygon);
            result.emplace_back(vertices, holes);
        }

        return result;
    }

    std::vector<Polygon> polygonUnion(const std::vector<Polygon>& polygons) noexcept {
        std::vector<Polygon> result;
        std::vector<polygon_t> bg_polygons;
        std::vector<polygon_t> bg_result;

        // Convert to Boost polygons
        for (const auto& polygon : polygons) {
            toBoostPolygon(bg_polygons.emplace_back(), polygon.getVertices(), polygon.getHoles());
        }

        // Union
        for (const polygon_t& bg_polygon : bg_polygons) {
            polygon_t current = bg_polygon;

            // Union the bg_polygon with every polygon in the result
            std::vector<bool> removeIndex(bg_result.size());
            for (size_t i = 0; i < bg_result.size(); i++) {
                std::vector<polygon_t> tempResult;
                bg::union_(current, bg_result[i], tempResult);
                removeIndex[i] = (tempResult.size() == 1);
                if (removeIndex[i]) {
                    current = tempResult[0];
                }
            }

            // Remove the polygons that were unioned
            std::erase_if(bg_result, [&](const polygon_t& bg_polygon) {
                return removeIndex[&bg_polygon - &bg_result[0]];
            });

            // Add the unioned polygon
            bg_result.push_back(current);
        }

        // Convert back to Polygon
        for (const auto& bg_polygon : bg_result) {
            std::vector<Vec2> vertices;
            std::vector<std::vector<Vec2>> holes;
            fromBoostPolygon(vertices, holes, bg_polygon);
            result.emplace_back(vertices, holes);
        }

        return result;
    }

    std::vector<Polygon> polygonIntersection(const Polygon& polygon1, const Polygon& polygon2) noexcept {
        std::vector<Polygon> result;
        polygon_t bg_polygon1, bg_polygon2;
        std::vector<polygon_t> bg_result;

        // Convert to Boost polygons
        toBoostPolygon(bg_polygon1, polygon1.getVertices(), polygon1.getHoles());
        toBoostPolygon(bg_polygon2, polygon2.getVertices(), polygon2.getHoles());

        // Intersection
        bg::intersection(bg_polygon1, bg_polygon2, bg_result);

        // Convert back to Polygon
        for (const auto& bg_polygon : bg_result) {
            std::vector<Vec2> vertices;
            std::vector<std::vector<Vec2>> holes;
            fromBoostPolygon(vertices, holes, bg_polygon);
            result.emplace_back(vertices, holes);
        }

        return result;
    }

    std::vector<Polygon> polygonIntersection(const std::vector<Polygon>& polygons) noexcept {
        std::vector<Polygon> result;
        std::vector<polygon_t> bg_polygons;
        std::vector<polygon_t> bg_result;

        // Convert to Boost polygons
        for (size_t i = 0; i < polygons.size(); i++) {
            // Add first polygon to bg_result
            polygon_t& next = (i == 0) ? bg_result.emplace_back() : bg_polygons.emplace_back();
            toBoostPolygon(next, polygons[i].getVertices(), polygons[i].getHoles());
        }

        // Intersection
        for (const polygon_t& bg_polygon : bg_polygons) {
            // If there are no polygons in the result then intersection is empty
            if (bg_result.empty()) {
                return result;
            }

            std::vector<polygon_t> newResult;

            // Intersect the bg_polygon with every piece in the result
            for (const polygon_t& piece : bg_result) {
                std::vector<polygon_t> tempResult;
                bg::intersection(bg_polygon, piece, tempResult);
                // And collect the results as a single list
                newResult.insert(newResult.end(), tempResult.begin(), tempResult.end());
            }

            // Update the new result
            bg_result = std::move(newResult);
        }

        // Convert back to Polygon
        for (const auto& bg_polygon : bg_result) {
            std::vector<Vec2> vertices;
            std::vector<std::vector<Vec2>> holes;
            fromBoostPolygon(vertices, holes, bg_polygon);
            result.emplace_back(vertices, holes);
        }

        return result;
    }

    std::vector<Polygon> polygonDifference(const Polygon& polygon1, const Polygon& polygon2) noexcept {
        std::vector<Polygon> result;
        polygon_t bg_polygon1, bg_polygon2;
        std::vector<polygon_t> bg_result;

        // Convert to Boost polygons
        toBoostPolygon(bg_polygon1, polygon1.getVertices(), polygon1.getHoles());
        toBoostPolygon(bg_polygon2, polygon2.getVertices(), polygon2.getHoles());

        // Difference
        bg::difference(bg_polygon1, bg_polygon2, bg_result);

        // Convert back to Polygon
        for (const auto& bg_polygon : bg_result) {
            std::vector<Vec2> vertices;
            std::vector<std::vector<Vec2>> holes;
            fromBoostPolygon(vertices, holes, bg_polygon);
            result.emplace_back(vertices, holes);
        }

        return result;
    }

    Polygon convexHull(const std::vector<Vec2>& points) noexcept {
        if (points.size() < 3) {
            return {};
        }

        polygon_t bg_polygon;
        bg::model::multi_point<point_t> bg_points;
        std::vector<Vec2> vertices;
        std::vector<std::vector<Vec2>> holes;

        // Convert the points to Boost points
        for (const Vec2& point : points) {
            bg::append(bg_points, point_t(point.x, point.y));
        }

        // Find convex hull
        bg::convex_hull(bg_points, bg_polygon);

        // Convert back to Polygon
        fromBoostPolygon(vertices, holes, bg_polygon);

        return { vertices, holes };
    }

    Polygon polygonSweep(const Polygon& polygon, const Vec2& displacement) {
        const std::vector<std::array<Vec2, 3>>& triangulation = polygon.getTriangles();
        std::vector<Polygon> sweptTriangles;
        sweptTriangles.reserve(triangulation.size());

        // Step 1: Sweep each triangle
        // The swept region of a triangle is the convex hull of the vertices before and after the displacement
        for (const auto& triangle : triangulation) {
            std::vector<Vec2> vertices = {
                triangle[0], triangle[1], triangle[2],
                triangle[0] + displacement, triangle[1] + displacement, triangle[2] + displacement,
            };
            sweptTriangles.push_back(convexHull(vertices));
        }

        // Step 2: Union the swept triangles
        // Note that the result must be a single polygon
        std::vector<Polygon> result = polygonUnion(sweptTriangles);
        if (result.size() != 1) {
            throw std::logic_error("Sweep resulted in multiple polygons");
        }

        return result[0];
    }
}

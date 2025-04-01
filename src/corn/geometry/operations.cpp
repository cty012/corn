#include <cmath>
#include <format>
#include <boost/geometry.hpp>
#include <corn/geometry/operations.h>
#include <corn/geometry/polygon.h>
#include "polygon_helper.h"

namespace corn {
    namespace bg = boost::geometry;
    using point_t = bg::model::d2::point_xy<float>;
    using polygon_t = bg::model::polygon<point_t>;
    using ring_t = bg::model::ring<point_t>;

    Vec2f rotate(const Vec2f& point, const Deg& deg) noexcept {
        float cosDeg = deg.cos();
        float sinDeg = deg.sin();
        return Vec2f(
            point.x * cosDeg - point.y * sinDeg,
            point.x * sinDeg + point.y * cosDeg
        );
    }

    Vec3f rotate(const Vec3f& point, const Quaternion& quaternion) noexcept {
        return (quaternion * Quaternion(point.append<1>(0.0f)) * quaternion.inv()).val.to<3>();
    }

    float triangleArea(const Vec2f& v1, const Vec2f& v2, const Vec2f& v3) noexcept {
        return 0.5f * std::abs(v1.x * (v2.y - v3.y) + v2.x * (v3.y - v1.y) + v3.x * (v1.y - v2.y));
    }

    float triangleArea(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) noexcept {
        Vec3f ab = v2 - v1;
        Vec3f ac = v3 - v1;
        Vec3f crossProduct = cross(ab, ac);
        return 0.5f * crossProduct.norm();
    }

    Vec2f triangleCentroid(const Vec2f& v1, const Vec2f& v2, const Vec2f& v3) noexcept {
        return (v1 + v2 + v3) * (1.0f / 3.0f);
    }

    Vec3f triangleCentroid(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) noexcept {
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

        // Convert back to Polygon type
        for (polygon_t& bg_polygon : bg_result) {
            // Simplify the polygon
//            polygon_t bg_polygon_simplified;
            bg::correct(bg_polygon);
//            bg::simplify(bg_polygon, bg_polygon_simplified, 0.0001f);

            std::vector<Vec2f> vertices;
            std::vector<std::vector<Vec2f>> holes;
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
                    bg::correct(current);
//                    bg::simplify(tempResult[0], current, 0.0001f);
                }
            }

            // Remove the polygons that were unioned
            size_t i = 0;
            std::erase_if(bg_result, [&](const polygon_t&) {
                return removeIndex[i++];
            });

            // Add the unioned polygon
            bg_result.push_back(current);
        }

        // Convert back to Polygon
        for (polygon_t& bg_polygon : bg_result) {
            bg::correct(bg_polygon);

            std::vector<Vec2f> vertices;
            std::vector<std::vector<Vec2f>> holes;
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
            std::vector<Vec2f> vertices;
            std::vector<std::vector<Vec2f>> holes;
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
            std::vector<Vec2f> vertices;
            std::vector<std::vector<Vec2f>> holes;
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
            std::vector<Vec2f> vertices;
            std::vector<std::vector<Vec2f>> holes;
            fromBoostPolygon(vertices, holes, bg_polygon);
            result.emplace_back(vertices, holes);
        }

        return result;
    }

    Polygon convexHull(const std::vector<Vec2f>& points) noexcept {
        if (points.size() < 3) {
            return {};
        }

        polygon_t bg_polygon;
        bg::model::multi_point<point_t> bg_points;
        std::vector<Vec2f> vertices;
        std::vector<std::vector<Vec2f>> holes;

        // Convert the points to Boost points
        for (const Vec2f& point : points) {
            bg::append(bg_points, point_t(point.x, point.y));
        }

        // Find convex hull
        bg::convex_hull(bg_points, bg_polygon);

        // Convert back to Polygon
        fromBoostPolygon(vertices, holes, bg_polygon);

        return { vertices, holes };
    }

    Polygon polygonSweep(const Polygon& polygon, const Vec2f& displacement) {
        const std::vector<std::array<Vec2f, 3>>& triangulation = polygon.getTriangles();
        std::vector<Polygon> sweptTriangles;
        sweptTriangles.reserve(triangulation.size());

        // Step 1: Sweep each triangle
        // The swept region of a triangle is the convex hull of the vertices before and after the displacement
        for (const auto& triangle : triangulation) {
            std::vector<Vec2f> vertices = {
                triangle[0], triangle[1], triangle[2],
                triangle[0] + displacement, triangle[1] + displacement, triangle[2] + displacement,
            };
            sweptTriangles.push_back(convexHull(vertices));
        }

        // Step 2: Union the swept triangles
        // Note that the result must be a single polygon
        std::vector<Polygon> result = polygonUnion(sweptTriangles);
        if (result.size() != 1) {
            // throw std::logic_error("Sweep resulted in multiple polygons");
            // If not a single polygon, return the largest one
            size_t i = 0;
            float maxArea = 0.0f;
            for (size_t j = 0; j < result.size(); j++) {
                float area = result[j].getArea();
                if (area > maxArea) {
                    i = j;
                    maxArea = area;
                }
            }
            return result[i];
        }

        return result[0];
    }
}

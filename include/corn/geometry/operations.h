#pragma once

#include <concepts>
#include <string>
#include <type_traits>
#include <vector>
#include <corn/geometry/rotation.h>
#include <corn/geometry/vec.h>

namespace corn {
    class Polygon;

    /**
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @return The vector formed by the minimum values in each direction.
     */
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    [[nodiscard]] Vec<T, N> min(const Vec<T, N>& v1, const Vec<T, N>& v2) {
        Vec<T, N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = std::min(v1[i], v2[i]);
        }
        return result;
    }

    /**
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @return The vector formed by the maximum values in each direction.
     */
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    [[nodiscard]] Vec<T, N> max(const Vec<T, N>& v1, const Vec<T, N>& v2) {
        Vec<T, N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = std::max(v1[i], v2[i]);
        }
        return result;
    }

    /**
     * @param minimum Minimum bound of the range.
     * @param value Original unclamped value.
     * @param maximum Maximum bound of the range.
     * @return The value in [minimum, maximum] closest to the original value.
     *
     * If minimum is greater than maximum, the original value will be returned.
     */
    template <typename T>
    requires(Numeric<T>)
    [[nodiscard]] T clamp(T minimum, T value, T maximum) {
        if (value < minimum) return minimum;
        else if (value > maximum) return maximum;
        else return value;
    }

    /**
     * @param minimum Minimum bound of the range.
     * @param value Original unclamped value.
     * @param maximum Maximum bound of the range.
     * @return The point inside rectangle formed by [minimum, maximum] closest to the original value.
     */
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    [[nodiscard]] Vec<T, N> clamp(const Vec<T, N>& minimum, const Vec<T, N>& value, const Vec<T, N>& maximum) {
        Vec<T, N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = clamp(minimum[i], value[i], maximum[i]);
        }
        return result;
    }

    /**
     * @param point The point to be rotated.
     * @param deg The angle of rotation in degrees.
     * @return The rotated point.
     */
    [[nodiscard]] Vec2f rotate(const Vec2f& point, const Deg& deg) noexcept;

    /**
     * @param point The point to be rotated.
     * @param quaternion The quaternion representing the rotation.
     * @return The rotated point.
     *
     * The quaternion is assumed to be normalized.
     */
    [[nodiscard]] Vec3f rotate(const Vec3f& point, const Quaternion& quaternion) noexcept;

    // Template definitions
    /**
     * @param v1 The first vertex.
     * @param v2 The second vertex.
     * @param v3 The third vertex.
     * @return Area of the triangle in 2D plane.
     */
    [[nodiscard]] float triangleArea(const Vec2f& v1, const Vec2f& v2, const Vec2f& v3) noexcept;

    /**
     * @param v1 The first vertex.
     * @param v2 The second vertex.
     * @param v3 The third vertex.
     * @return Area of the triangle in 3D plane.
     */
    [[nodiscard]] float triangleArea(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) noexcept;

    /**
     * @param v1 The first vertex.
     * @param v2 The second vertex.
     * @param v3 The third vertex.
     * @return Centroid of the triangle in 2D plane.
     */
    [[nodiscard]] Vec2f triangleCentroid(const Vec2f& v1, const Vec2f& v2, const Vec2f& v3) noexcept;

    /**
     * @param v1 The first vertex.
     * @param v2 The second vertex.
     * @param v3 The third vertex.
     * @return Centroid of the triangle in 3D plane.
     */
    [[nodiscard]] Vec3f triangleCentroid(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) noexcept;

    // Polygon operations
    /**
     * @param polygon1 The first polygon.
     * @param polygon2 The second polygon.
     * @return The union of the two polygons.
     */
    [[nodiscard]] std::vector<Polygon> polygonUnion(const Polygon& polygon1, const Polygon& polygon2) noexcept;

    /**
     * @param polygons The list of polygons.
     * @return The union of the list of polygons.
     */
    [[nodiscard]] std::vector<Polygon> polygonUnion(const std::vector<Polygon>& polygons) noexcept;

    /**
     * @param polygon1 The first polygon.
     * @param polygon2 The second polygon.
     * @return The intersection of the two polygons.
     */
    [[nodiscard]] std::vector<Polygon> polygonIntersection(const Polygon& polygon1, const Polygon& polygon2) noexcept;

    /**
     * @param polygons The list of polygons.
     * @return The intersection of the list of polygons.
     */
    [[nodiscard]] std::vector<Polygon> polygonIntersection(const std::vector<Polygon>& polygons) noexcept;

    /**
     * @param polygon1 The first polygon.
     * @param polygon2 The second polygon.
     * @return The difference of the two polygons (first minus second).
     */
    [[nodiscard]] std::vector<Polygon> polygonDifference(const Polygon& polygon1, const Polygon& polygon2) noexcept;

    /**
     * @param points The set of points.
     * @return The convex hull of the set of points.
     */
    [[nodiscard]] Polygon convexHull(const std::vector<Vec2f>& points) noexcept;

    /**
     * @param polygon The polygon.
     * @param displacement The displacement vector.
     * @return The polygon swept by the displacement vector.
     */
    [[nodiscard]] Polygon polygonSweep(const Polygon& polygon, const Vec2f& displacement);
}

#pragma once

#include <concepts>
#include <type_traits>

namespace corn {
    struct Deg;
    struct Quaternion;
    struct Vec2;
    struct Vec3;
    struct Vec4;

    template <typename T>
    concept Num = std::is_same_v<T, int> || std::is_same_v<T, size_t> || std::is_same_v<T, float>;

    template <typename Vec>
    concept VectorType = std::is_same_v<Vec, Vec2> || std::is_same_v<Vec, Vec3> || std::is_same_v<Vec, Vec4>;

    /**
     * @tparam T Numeric type. Can be int, size_t, or float.
     * @param minimum Minimum bound of the range.
     * @param value Original unclamped value.
     * @param maximum Maximum bound of the range.
     * @return The value in [minimum, maximum] closest to the original value.
     *
     * If minimum is greater than maximum, the original value will be returned.
     */
    template <Num T>
    T clamp(T minimum, T value, T maximum) {
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
    Vec2 clamp(const Vec2& minimum, const Vec2& value, const Vec2& maximum);

    /**
     * @param minimum Minimum bound of the range.
     * @param value Original unclamped value.
     * @param maximum Maximum bound of the range.
     * @return The point inside rectangle formed by [minimum, maximum] closest to the original value.
     */
    Vec3 clamp(const Vec3& minimum, const Vec3& value, const Vec3& maximum);

    /**
     * @param minimum Minimum bound of the range.
     * @param value Original unclamped value.
     * @param maximum Maximum bound of the range.
     * @return The point inside rectangle formed by [minimum, maximum] closest to the original value.
     */
    Vec4 clamp(const Vec4& minimum, const Vec4& value, const Vec4& maximum);

    /// @return Dot product of lhs and rhs of type Vec2.
    [[nodiscard]] float dot(const Vec2& lhs, const Vec2& rhs) noexcept;

    /// @return Dot product of lhs and rhs of type Vec3.
    [[nodiscard]] float dot(const Vec3& lhs, const Vec3& rhs) noexcept;

    /// @return Dot product of lhs and rhs of type Vec4.
    [[nodiscard]] float dot(const Vec4& lhs, const Vec4& rhs) noexcept;

    /**
     * @param v1 First 3D vector.
     * @param v2 Second 3D vector.
     * @return Cross product of the two vectors.
     */
    [[nodiscard]] Vec3 cross(const Vec3& v1, const Vec3& v2) noexcept;

    /**
     * @param v1 First vector.
     * @param v2 Second vector.
     * @return Euclidean distance between the two vectors.
     */
    template <VectorType Vec>
    [[nodiscard]] float dist(const Vec& v1, const Vec& v2) noexcept;

    /// @brief Rotate a 2D point.
    [[nodiscard]] Vec2 rotate(const Vec2& point, const Deg& deg) noexcept;
    /// @brief Rotate a 3D point.
    [[nodiscard]] Vec3 rotate(const Vec3& point, const Quaternion& quaternion) noexcept;

    // Template definitions
    template <VectorType Vec>
    float dist(const Vec& v1, const Vec& v2) noexcept {
        return (v1 - v2).norm();
    }

    /**
     * @param v1 The first vertex.
     * @param v2 The second vertex.
     * @param v3 The third vertex.
     * @return Area of the triangle in 2D plane.
     */
    [[nodiscard]] float area(const Vec2& v1, const Vec2& v2, const Vec2& v3) noexcept;

    /**
     * @param v1 The first vertex.
     * @param v2 The second vertex.
     * @param v3 The third vertex.
     * @return Area of the triangle in 3D plane.
     */
    [[nodiscard]] float area(const Vec3& v1, const Vec3& v2, const Vec3& v3) noexcept;

    /**
     * @param v1 The first vertex.
     * @param v2 The second vertex.
     * @param v3 The third vertex.
     * @return Centroid of the triangle in 2D plane.
     */
    [[nodiscard]] Vec2 centroid(const Vec2& v1, const Vec2& v2, const Vec2& v3) noexcept;

    /**
     * @param v1 The first vertex.
     * @param v2 The second vertex.
     * @param v3 The third vertex.
     * @return Centroid of the triangle in 3D plane.
     */
    [[nodiscard]] Vec3 centroid(const Vec3& v1, const Vec3& v2, const Vec3& v3) noexcept;
}

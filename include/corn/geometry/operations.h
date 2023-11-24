#pragma once

#include <concepts>
#include <type_traits>
#include <corn/geometry/deg.h>
#include <corn/geometry/rotation.h>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
    template <typename Vec>
    concept VectorType = std::is_same_v<Vec, Vec2> || std::is_same_v<Vec, Vec3> || std::is_same_v<Vec, Vec4>;

    /// @return Dot product of lhs and rhs of type Vec2.
    float dot(const Vec2& lhs, const Vec2& rhs);
    /// @return Dot product of lhs and rhs of type Vec3.
    float dot(const Vec3& lhs, const Vec3& rhs);
    /// @return Dot product of lhs and rhs of type Vec4.
    float dot(const Vec4& lhs, const Vec4& rhs);

    /**
     * @param v1 First 3D vector.
     * @param v2 Second 3D vector.
     * @return Cross product of the two vectors.
     */
    Vec3 cross(const Vec3& v1, const Vec3& v2);

    /**
     * @param v1 First vector.
     * @param v2 Second vector.
     * @return Euclidean distance between the two vectors.
     */
    template <VectorType Vec>
    float dist(const Vec& v1, const Vec& v2);

    /// @brief Rotate a 2D point.
    [[nodiscard]] Vec2 rotate(const Vec2& point, const Deg& deg);
    /// @brief Rotate a 3D point.
    [[nodiscard]] Vec3 rotate(const Vec3& point, const Quaternion& quaternion);

    // Template definitions
    template <VectorType Vec>
    float dist(const Vec& v1, const Vec& v2) {
        return (v1 - v2).norm();
    }
}

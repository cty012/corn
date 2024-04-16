#include <cmath>
#include <corn/geometry/operations.h>
#include <corn/geometry/rotation.h>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
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

    float area(const Vec2& v1, const Vec2& v2, const Vec2& v3) noexcept {
        return 0.5f * std::abs(v1.x * (v2.y - v3.y) + v2.x * (v3.y - v1.y) + v3.x * (v1.y - v2.y));
    }

    float area(const Vec3& v1, const Vec3& v2, const Vec3& v3) noexcept {
        Vec3 ab = v2 - v1;
        Vec3 ac = v3 - v1;
        Vec3 crossProduct = cross(ab, ac);
        return 0.5f * crossProduct.norm();
    }

    Vec2 centroid(const Vec2& v1, const Vec2& v2, const Vec2& v3) noexcept {
        return (v1 + v2 + v3) * (1.0f / 3.0f);
    }

    Vec3 centroid(const Vec3& v1, const Vec3& v2, const Vec3& v3) noexcept {
        return (v1 + v2 + v3) * (1.0f / 3.0f);
    }
}

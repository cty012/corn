#include <corn/geometry/operations.h>

namespace corn {
    float dot(const Vec2& lhs, const Vec2& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    float dot(const Vec3& lhs, const Vec3& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    float dot(const Vec4& lhs, const Vec4& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
    }

    Vec3 cross(const Vec3& v1, const Vec3& v2) {
        return {
            v1.y * v2.z - v2.y * v1.z,
            v1.z * v2.x - v2.z * v1.x,
            v1.x * v2.y - v2.x * v1.y,
        };
    }

    Vec2 rotate(const Vec2& point, const Deg& deg) {
        float cdeg = deg.cos();
        float sdeg = deg.sin();
        return { point.x * cdeg + point.y * sdeg, -point.x * sdeg + point.y * cdeg };
    }

    Vec3 rotate(const Vec3& point, const Quaternion& quaternion) {
        Quaternion p = { 0, point.x, point.y, point.z };
        Vec4 result = (quaternion * p * quaternion.inv()).val;
        return { result.y, result.z, result.w };  // NOLINT
    }
}

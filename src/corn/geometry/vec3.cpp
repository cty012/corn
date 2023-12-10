#include <cmath>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
    Vec3::Vec3() noexcept : x(0.0), y(0.0), z(0.0) {}

    Vec3::Vec3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

    const Vec3& Vec3::ZERO() noexcept {
        static const Vec3 zero = { 0.0f, 0.0f, 0.0f };
        return zero;
    }

    const Vec3& Vec3::UNIT_X() noexcept {
        static const Vec3 unitX = { 1.0f, 0.0f, 0.0f };
        return unitX;
    }

    const Vec3& Vec3::UNIT_Y() noexcept {
        static const Vec3 unitY = { 0.0f, 1.0f, 0.0f };
        return unitY;
    }

    const Vec3& Vec3::UNIT_Z() noexcept {
        static const Vec3 unitZ = { 0.0f, 0.0f, 1.0f };
        return unitZ;
    }

    Vec2 Vec3::vec2() const noexcept {
        return { this->x, this->y };
    }

    Vec4 Vec3::vec4(float w) const noexcept {
        return { this->x, this->y, this->z, w };
    }

    float Vec3::norm() const noexcept {
        return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    }

    Vec3 Vec3::normalize() const noexcept {
        float n = this->norm();
        return n == 0 ? *this : *this * (1 / n);
    }

    Vec3 operator+(const Vec3& rhs) noexcept {
        return rhs;
    }

    Vec3 operator-(const Vec3& rhs) noexcept {
        return { -rhs.x, -rhs.y, -rhs.z };
    }

    Vec3 operator+(const Vec3& lhs, const Vec3& rhs) noexcept {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }

    Vec3 operator-(const Vec3& lhs, const Vec3& rhs) noexcept {
        return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
    }

    Vec3 operator*(const Vec3& lhs, const Vec3& rhs) noexcept {
        return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
    }

    Vec3 operator*(const Vec3& vec, float scalar) noexcept {
        return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
    }

    Vec3 operator*(float scalar, const Vec3& vec) noexcept {
        return { scalar * vec.x, scalar * vec.y, scalar * vec.z };
    }

    Vec3& operator+=(Vec3& lhs, const Vec3& rhs) noexcept {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        return lhs;
    }

    Vec3& operator-=(Vec3& lhs, const Vec3& rhs) noexcept {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        return lhs;
    }

    Vec3& operator*=(Vec3& lhs, const Vec3& rhs) noexcept {
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        lhs.z *= rhs.z;
        return lhs;
    }
}

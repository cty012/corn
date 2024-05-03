#include <cmath>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
    Vec4::Vec4() noexcept : x(0.0), y(0.0), z(0.0), w(0.0) {}

    Vec4::Vec4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}

    Vec2 Vec4::vec2() const noexcept {
        return { this->x, this->y };
    }

    Vec3 Vec4::vec3() const noexcept {
        return { this->x, this->y, this->z };
    }

    const Vec4& Vec4::ZERO() noexcept {
        static const Vec4 zero = { 0.0f, 0.0f, 0.0f, 0.0f };
        return zero;
    }

    const Vec4& Vec4::UNIT_X() noexcept {
        static const Vec4 unitX = { 1.0f, 0.0f, 0.0f, 0.0f };
        return unitX;
    }

    const Vec4& Vec4::UNIT_Y() noexcept {
        static const Vec4 unitY = { 0.0f, 1.0f, 0.0f, 0.0f };
        return unitY;
    }

    const Vec4& Vec4::UNIT_Z() noexcept {
        static const Vec4 unitZ = { 0.0f, 0.0f, 1.0f, 0.0f };
        return unitZ;
    }

    const Vec4& Vec4::UNIT_W() noexcept {
        static const Vec4 unitZ = { 0.0f, 0.0f, 0.0f, 1.0f };
        return unitZ;
    }

    float Vec4::norm() const noexcept {
        return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
    }

    Vec4 Vec4::normalize() const noexcept {
        float n = this->norm();
        return n == 0 ? *this : *this * (1 / n);
    }

    Vec4 operator+(const Vec4& rhs) noexcept {
        return rhs;
    }

    Vec4 operator-(const Vec4& rhs) noexcept {
        return { -rhs.x, -rhs.y, -rhs.z, -rhs.w };
    }

    Vec4 operator+(const Vec4& lhs, const Vec4& rhs) noexcept {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
    }

    Vec4 operator-(const Vec4& lhs, const Vec4& rhs) noexcept {
        return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
    }

    Vec4 operator*(const Vec4& lhs, const Vec4& rhs) noexcept {
        return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
    }

    Vec4 operator*(const Vec4& vec, float scalar) noexcept {
        return { vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar };
    }

    Vec4 operator*(float scalar, const Vec4& vec) noexcept {
        return { scalar * vec.x, scalar * vec.y, scalar * vec.z, scalar * vec.w };
    }

    Vec4& operator+=(Vec4& lhs, const Vec4& rhs) noexcept {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        lhs.w += rhs.w;
        return lhs;
    }

    Vec4& operator-=(Vec4& lhs, const Vec4& rhs) noexcept {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        lhs.w -= rhs.w;
        return lhs;
    }

    Vec4& operator*=(Vec4& vec, float scalar) noexcept {
        vec.x *= scalar;
        vec.y *= scalar;
        vec.z *= scalar;
        vec.w *= scalar;
        return vec;
    }

    Vec4& operator*=(Vec4& lhs, const Vec4& rhs) noexcept {
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        lhs.z *= rhs.z;
        lhs.w *= rhs.w;
        return lhs;
    }
}

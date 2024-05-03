#include <cmath>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
    Vec2::Vec2() noexcept : x(0.0), y(0.0) {}

    Vec2::Vec2(float x, float y) noexcept : x(x), y(y) {}

    const Vec2& Vec2::ZERO() noexcept {
        static const Vec2 zero = { 0.0f, 0.0f };
        return zero;
    }

    const Vec2& Vec2::UNIT_X() noexcept {
        static const Vec2 unitX = { 1.0f, 0.0f };
        return unitX;
    }

    const Vec2& Vec2::UNIT_Y() noexcept {
        static const Vec2 unitY = { 0.0f, 1.0f };
        return unitY;
    }

    Vec3 Vec2::vec3(float z) const noexcept {
        return { this->x, this->y, z };
    }

    Vec4 Vec2::vec4(float z, float w) const noexcept {
        return { this->x, this->y, z, w };
    }

    float Vec2::norm() const noexcept {
        return std::sqrt(this->x * this->x + this->y * this->y);
    }

    Vec2 Vec2::normalize() const noexcept {
        float n = this->norm();
        return n == 0 ? *this : *this * (1 / n);
    }

    Vec2 operator+(const Vec2& rhs) noexcept {
        return rhs;
    }

    Vec2 operator-(const Vec2& rhs) noexcept {
        return { -rhs.x, -rhs.y };
    }

    Vec2 operator+(const Vec2& lhs, const Vec2& rhs) noexcept {
        return { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    Vec2 operator-(const Vec2& lhs, const Vec2& rhs) noexcept {
        return { lhs.x - rhs.x, lhs.y - rhs.y };
    }

    Vec2 operator*(const Vec2& lhs, const Vec2& rhs) noexcept {
        return { lhs.x * rhs.x, lhs.y * rhs.y };
    }

    Vec2 operator*(const Vec2& vec, float scalar) noexcept {
        return { vec.x * scalar, vec.y * scalar };
    }

    Vec2 operator*(float scalar, const Vec2& vec) noexcept {
        return { scalar * vec.x, scalar * vec.y };
    }

    Vec2& operator+=(Vec2& lhs, const Vec2& rhs) noexcept {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        return lhs;
    }

    Vec2& operator-=(Vec2& lhs, const Vec2& rhs) noexcept {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        return lhs;
    }

    Vec2& operator*=(Vec2& vec, float scalar) noexcept {
        vec.x *= scalar;
        vec.y *= scalar;
        return vec;
    }

    Vec2& operator*=(Vec2& lhs, const Vec2& rhs) noexcept {
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        return lhs;
    }
}

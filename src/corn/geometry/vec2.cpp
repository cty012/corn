#include <cmath>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
    Vec2::Vec2() : x(0.0), y(0.0) {}

    Vec2::Vec2(float x, float y) : x(x), y(y) {}

    const Vec2& Vec2::ZERO() {
        static const Vec2 zero = { 0.0f, 0.0f };
        return zero;
    }

    const Vec2& Vec2::UNIT_X() {
        static const Vec2 unitX = { 1.0f, 0.0f };
        return unitX;
    }

    const Vec2& Vec2::UNIT_Y() {
        static const Vec2 unitY = { 0.0f, 1.0f };
        return unitY;
    }

    Vec3 Vec2::vec3(float z) const {
        return { this->x, this->y, z };
    }

    Vec4 Vec2::vec4(float z, float w) const {
        return { this->x, this->y, z, w };
    }

    float Vec2::norm() const {
        return std::sqrt(this->x * this->x + this->y * this->y);
    }

    Vec2 Vec2::normalize() const {
        float n = this->norm();
        return n == 0 ? *this : *this * (1 / n);
    }

    Vec2 operator+(const Vec2& rhs) {
        return rhs;
    }

    Vec2 operator-(const Vec2& rhs) {
        return { -rhs.x, -rhs.y };
    }

    Vec2 operator+(const Vec2& lhs, const Vec2& rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    Vec2 operator-(const Vec2& lhs, const Vec2& rhs) {
        return { lhs.x - rhs.x, lhs.y - rhs.y };
    }

    Vec2 operator*(const Vec2& lhs, const Vec2& rhs) {
        return { lhs.x * rhs.x, lhs.y * rhs.y };
    }

    Vec2 operator*(const Vec2& vec, float scalar) {
        return { vec.x * scalar, vec.y * scalar };
    }

    Vec2 operator*(float scalar, const Vec2& vec) {
        return { scalar * vec.x, scalar * vec.y };
    }

    Vec2& operator+=(Vec2& lhs, const Vec2& rhs) {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        return lhs;
    }

    Vec2& operator-=(Vec2& lhs, const Vec2& rhs) {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        return lhs;
    }

    Vec2& operator*=(Vec2& lhs, const Vec2& rhs) {
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        return lhs;
    }
}

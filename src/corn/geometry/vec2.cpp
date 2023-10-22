#include <cmath>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
    Vec2::Vec2() : x(0.0), y(0.0) {}

    Vec2::Vec2(float x, float y) : x(x), y(y) {}

    const Vec2& Vec2::ZERO() {
        static Vec2 zero = {0.0f, 0.0f};
        return zero;
    }

    const Vec2& Vec2::UNIT_X() {
        static Vec2 unitX = { 1.0f, 0.0f };
        return unitX;
    }

    const Vec2& Vec2::UNIT_Y() {
        static Vec2 unitY = { 0.0f, 1.0f };
        return unitY;
    }

    Vec3 Vec2::vec3(float z) const {
        return {this->x, this->y, z};
    }

    Vec4 Vec2::vec4(float z, float w) const {
        return {this->x, this->y, z, w};
    }

    Vec2 Vec2::operator+() const {
        return *this;
    }

    Vec2 Vec2::operator-() const {
        return {-this->x, -this->y};
    }

    Vec2 Vec2::operator+(const Vec2& other) const {
        return {this->x + other.x, this->y + other.y};
    }

    Vec2 Vec2::operator-(const Vec2& other) const {
        return {this->x - other.x, this->y - other.y};
    }

    Vec2& Vec2::operator+=(const Vec2& other) {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    Vec2& Vec2::operator-=(const Vec2& other) {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }

    float Vec2::dot(const Vec2& other) const {
        return this->x * other.x + this->y * other.y;
    }

    Vec2 Vec2::operator*(const Vec2& other) const {
        return {this->x * other.x, this->y * other.y};
    }

    Vec2 Vec2::operator*=(const Vec2& other) {
        this->x *= other.x;
        this->y *= other.y;
        return *this;
    }

    Vec2 Vec2::mult(float factor) const {
        return {this->x * factor, this->y * factor};
    }

    float Vec2::norm() const {
        return std::sqrt(this->dot(*this));
    }

    Vec2 Vec2::normalize() const {
        float n = this->norm();
        return n == 0 ? *this : this->mult(1 / n);
    }
}

#include <cmath>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
    const Vec2 Vec2::ZERO = Vec2(0.0, 0.0);
    const Vec2 Vec2::UNIT_X = Vec2(1.0, 0.0);
    const Vec2 Vec2::UNIT_Y = Vec2(0.0, 1.0);

    Vec2::Vec2() : x(0.0), y(0.0) {}

    Vec2::Vec2(double x, double y) : x(x), y(y) {}

    Vec3 Vec2::vec3(double z) const {
        return {this->x, this->y, z};
    }

    Vec4 Vec2::vec4(double z, double w) const {
        return {this->x, this->y, z, w};
    }

    Vec2 Vec2::operator+() const {
        return *this;
    }

    Vec2 Vec2::operator-() const {
        return {-this->x, -this->y};
    }

    Vec2 Vec2::operator+(const Vec2 &other) const {
        return {this->x + other.x, this->y + other.y};
    }

    Vec2 Vec2::operator-(const Vec2 &other) const {
        return {this->x - other.x, this->y - other.y};
    }

    Vec2& Vec2::operator+=(const Vec2 &other) {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    Vec2& Vec2::operator-=(const Vec2 &other) {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }

    double Vec2::dot(const Vec2 &other) const {
        return this->x * other.x + this->y * other.y;
    }

    Vec2 Vec2::operator*(const Vec2 &other) const {
        return {this->x * other.x, this->y * other.y};
    }

    Vec2 Vec2::mult(double factor) const {
        return {this->x * factor, this->y * factor};
    }

    double Vec2::norm() const {
        return std::sqrt(this->dot(*this));
    }

    Vec2 Vec2::normalize() const {
        double n = this->norm();
        return n == 0 ? *this : this->mult(1 / n);
    }
}

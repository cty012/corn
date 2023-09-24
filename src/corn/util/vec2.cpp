#include <cmath>
#include <corn/util/geometry.h>

namespace corn {
    const Vec2 Vec2::ZERO = Vec2(0.0, 0.0);
    const Vec2 Vec2::UNIT_X = Vec2(1.0, 0.0);
    const Vec2 Vec2::UNIT_Y = Vec2(0.0, 1.0);

    Vec2::Vec2() : x(0.0), y(0.0) {}

    Vec2::Vec2(double x, double y) : x(x), y(y) {}

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

    double dist(const Vec2& v1, const Vec2& v2) {
        return (v1 - v2).norm();
    }
}

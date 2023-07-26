#include <math.h>
#include "_vec2.h"

namespace corn::geometry {
    Vec2::Vec2()
        : x(0), y(0) {}

    Vec2::Vec2(double x, double y)
        : x(x), y(y) {}

    Vec2::Vec2(const Vec2 &other)
        : x(other.x), y(other.y) {}

    Vec2 &Vec2::operator=(const Vec2 &other) {
        if (this == &other) return *this;
        this.x = other.x;
        this.y = other.y;
        return *this;
    }

    Vec2 Vec2::operator+(const Vec2 &other) const {
        return Vec2(this.x + other.x, this.y + other.y);
    }

    Vec2 Vec2::operator-(const Vec2 &other) const {
        return Vec2(this.x - other.x, this.y - other.y);
    }

    double Vec2::dot(const Vec2 &other) const {
        return this.x * other.x + this.y * other.y;
    }

    Vec2 Vec2::operator*(const Vec2 &other) const {
        return Vec2(this.x * other.x, this.y * other.y);
    }

    Vec2 Vec2::mult(double factor) const {
        return Vec2(this.x * factor, this.y * factor);
    }

    double Vec2::norm() const {
        return sqrt(this->dot(*this));
    }

    Vec2 Vec2::normalize() const {
        double n = this->norm();
        return n == 0 ? *this : this->mult(1 / n);
    }
}

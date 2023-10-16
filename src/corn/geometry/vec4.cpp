#include <cmath>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
    Vec4::Vec4() : x(0.0), y(0.0), z(0.0), w(0.0) {}

    Vec4::Vec4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

    Vec2 Vec4::vec2() const {
        return {this->x, this->y};
    }

    Vec3 Vec4::vec3() const {
        return {this->x, this->y, this->z};
    }

    Vec4 Vec4::operator+() const {
        return *this;
    }

    Vec4 Vec4::operator-() const {
        return {-this->x, -this->y, -this->z, -this->w};
    }

    Vec4 Vec4::operator+(const Vec4& other) const {
        return {this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w};
    }

    Vec4 Vec4::operator-(const Vec4& other) const {
        return {this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w};
    }

    Vec4& Vec4::operator+=(const Vec4& other) {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        this->w += other.w;
        return *this;
    }

    Vec4& Vec4::operator-=(const Vec4& other) {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        this->w -= other.w;
        return *this;
    }

    double Vec4::dot(const Vec4& other) const {
        return this->x * other.x + this->y * other.y + this->z * other.z + this->w * other.w;
    }

    Vec4 Vec4::operator*(const Vec4& other) const {
        return {this->x * other.x, this->y * other.y, this->z * other.z, this->w * other.w};
    }

    Vec4 Vec4::mult(double factor) const {
        return {this->x * factor, this->y * factor, this->z * factor, this->w * factor};
    }

    double Vec4::norm() const {
        return std::sqrt(this->dot(*this));
    }

    Vec4 Vec4::normalize() const {
        double n = this->norm();
        return n == 0 ? *this : this->mult(1 / n);
    }
}

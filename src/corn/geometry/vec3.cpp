#include <cmath>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
    const Vec3 Vec3::ZERO = Vec3(0.0, 0.0, 0.0);
    const Vec3 Vec3::UNIT_X = Vec3(1.0, 0.0, 0.0);
    const Vec3 Vec3::UNIT_Y = Vec3(0.0, 1.0, 0.0);
    const Vec3 Vec3::UNIT_Z = Vec3(0.0, 0.0, 1.0);

    Vec3::Vec3() : x(0.0), y(0.0), z(0.0) {}

    Vec3::Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    Vec2 Vec3::vec2() const {
        return {this->x, this->y};
    }

    Vec4 Vec3::vec4(double w) const {
        return {this->x, this->y, this->z, w};
    }

    Vec3 Vec3::operator+() const {
        return *this;
    }

    Vec3 Vec3::operator-() const {
        return {-this->x, -this->y, -this->z};
    }

    Vec3 Vec3::operator+(const Vec3 &other) const {
        return {this->x + other.x, this->y + other.y, this->z + other.z};
    }

    Vec3 Vec3::operator-(const Vec3 &other) const {
        return {this->x - other.x, this->y - other.y, this->z - other.z};
    }

    Vec3& Vec3::operator+=(const Vec3 &other) {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }

    Vec3& Vec3::operator-=(const Vec3 &other) {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        return *this;
    }

    double Vec3::dot(const Vec3 &other) const {
        return this->x * other.x + this->y * other.y + this->z * other.z;
    }

    Vec3 Vec3::operator*(const Vec3 &other) const {
        return {this->x * other.x, this->y * other.y, this->z * other.z};
    }

    Vec3 Vec3::mult(double factor) const {
        return {this->x * factor, this->y * factor, this->z * factor};
    }

    double Vec3::norm() const {
        return std::sqrt(this->dot(*this));
    }

    Vec3 Vec3::normalize() const {
        double n = this->norm();
        return n == 0 ? *this : this->mult(1 / n);
    }
}

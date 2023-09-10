#include <cmath>
#include <corn/util/geometry.h>

namespace corn {
    Vec3::Vec3()
        : x(0.0), y(0.0), z(0.0) {}

    Vec3::Vec3(double x, double y, double z)
        : x(x), y(y), z(z) {}

    Vec3::Vec3(const Vec3 &other)
        : x(other.x), y(other.y), z(other.z) {}

    Vec3 &Vec3::operator=(const Vec3 &other) {
        if (this == &other) return *this;
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
        return *this;
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

    double dist(const Vec3& v1, const Vec3& v2) {
        return (v1 - v2).norm();
    }
}

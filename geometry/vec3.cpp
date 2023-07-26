#include <math.h>
#include "vec3.h"

namespace corn::geometry {
    Vec3::Vec3()
        : x(0), y(0), z(0) {}

    Vec3::Vec3(double x, double y, double z)
        : x(x), y(y), z(z) {}

    Vec3::Vec3(const Vec3 &other)
        : x(other.x), y(other.y), z(other.z) {}

    Vec3 &Vec3::operator=(const Vec3 &other) {
        if (this == &other) return *this;
        this.x = other.x;
        this.y = other.y;
        this.z = other.z;
        return *this;
    }

    Vec3 Vec3::operator+(const Vec3 &other) const {
        return Vec3(this.x + other.x, this.y + other.y, this.z + other.z);
    }

    Vec3 Vec3::operator-(const Vec3 &other) const {
        return Vec3(this.x - other.x, this.y - other.y, this.z - other.z);
    }

    double Vec3::dot(const Vec3 &other) const {
        return this.x * other.x + this.y * other.y + this.z * other.z;
    }

    Vec3 Vec3::operator*(const Vec3 &other) const {
        return Vec3(this.x * other.x, this.y * other.y, this.z * other.z);
    }

    Vec3 Vec3::mult(double factor) const {
        return Vec3(this.x * factor, this.y * factor, this.z * factor);
    }

    double Vec3::norm() const {
        return sqrt(this->dot(*this));
    }

    Vec3 Vec3::normalize() const {
        double n = this->norm();
        return n == 0 ? *this : this->mult(1 / n);
    }
}

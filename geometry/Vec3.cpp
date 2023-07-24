#include <math.h>
#include "Vec3.h"

namespace corn {
    namespace geometry {
        Vec3::Vec3() : Vec3(0, 0, 0) {}

        Vec3::Vec3(double x, double y, double z) {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        Vec3::Vec3(const Vec3 &other) : Vec3(other.x, other.y, other.z) {}

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
}

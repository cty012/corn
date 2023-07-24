#pragma once

namespace corn {
    namespace geometry {
        // 3D vector class
        class Vec3 {
        public:
            double x;
            double y;
            double z;

            // Ctors & dtors
            Vec3();
            Vec3(double x, double y, double z);
            ~Vec3() = default;
            Vec3(const Vec3 &other);
            Vec3 &operator=(const Vec3 &other);

            // Operations
            Vec3 operator+(const Vec3 &other) const;
            Vec3 operator-(const Vec3 &other) const;
            double dot(const Vec3 &other) const;
            Vec3 operator*(const Vec3 &other) const;
            Vec3 mult(double factor) const;
            double norm() const;
            Vec3 normalize() const;
        };
    }
}

#pragma once

namespace corn::geometry {
    // 2D vector class
    // Used for positions and sizes in 2D
    class Vec2 {
    public:
        double x;
        double y;

        // Ctors & dtors
        Vec2();
        Vec2(double x, double y);
        ~Vec2() = default;
        Vec2(const Vec2 &other);
        Vec2 &operator=(const Vec2 &other);

        // Operations
        Vec2 operator+(const Vec2 &other) const;
        Vec2 operator-(const Vec2 &other) const;
        double dot(const Vec2 &other) const;
        Vec2 operator*(const Vec2 &other) const;
        Vec2 mult(double factor) const;
        double norm() const;
        Vec2 normalize() const;
    };
}

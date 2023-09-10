#pragma once

namespace corn {
    /**
     * 2D vector class
     * Used for positions and sizes in 2D
     */
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
        Vec2 operator+() const;
        Vec2 operator-() const;
        Vec2 operator+(const Vec2 &other) const;
        Vec2 operator-(const Vec2 &other) const;
        Vec2& operator+=(const Vec2 &other);
        Vec2& operator-=(const Vec2 &other);

        double dot(const Vec2 &other) const;
        Vec2 operator*(const Vec2 &other) const;
        Vec2 mult(double factor) const;

        double norm() const;
        Vec2 normalize() const;
        friend double dist(const Vec2& v1, const Vec2& v2);
    };

    /**
     * 3D vector class
     */
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
        Vec3 operator+() const;
        Vec3 operator-() const;
        Vec3 operator+(const Vec3 &other) const;
        Vec3 operator-(const Vec3 &other) const;
        Vec3& operator+=(const Vec3 &other);
        Vec3& operator-=(const Vec3 &other);

        double dot(const Vec3 &other) const;
        Vec3 operator*(const Vec3 &other) const;
        Vec3 mult(double factor) const;

        double norm() const;
        Vec3 normalize() const;
        friend double dist(const Vec3& v1, const Vec3& v2);
    };
}

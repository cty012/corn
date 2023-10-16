#pragma once

namespace corn {
    struct Vec3;

    /**
     * @class Vec2
     * @brief 2D vector class.
     */
    struct Vec2 {
        double x;
        double y;
        static const Vec2 ZERO;
        static const Vec2 UNIT_X;
        static const Vec2 UNIT_Y;

        // Ctors & dtors
        Vec2();
        Vec2(double x, double y);

        // Conversions
        Vec3 vec3(double z) const;
//        Vec4 vec4(double z, double w) const;

        // Operations
        /// @return A copy of the vector itself.
        Vec2 operator+() const;
        /// @return The reversed vector.
        Vec2 operator-() const;
        /// @return Result of adding this and other.
        Vec2 operator+(const Vec2 &other) const;
        /// @return Result of subtracting other from this.
        Vec2 operator-(const Vec2 &other) const;
        /// @return Add other to this vector.
        Vec2& operator+=(const Vec2 &other);
        /// @return Subtract other from this vector.
        Vec2& operator-=(const Vec2 &other);

        /// @return Dot product of this and other.
        [[nodiscard]] double dot(const Vec2 &other) const;
        /// @return Element-wise multiplication of this and other.
        Vec2 operator*(const Vec2 &other) const;
        /// @return Results of multiplying by a scalar.
        [[nodiscard]] Vec2 mult(double factor) const;

        /// @return 2-norm of the vector.
        [[nodiscard]] double norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec2 normalize() const;
    };
}

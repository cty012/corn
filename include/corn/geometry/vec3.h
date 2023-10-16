#pragma once

namespace corn {
    struct Vec2;
    struct Vec4;

    /**
     * @class Vec3
     * @brief 3D vector class.
     */
    struct Vec3 {
        double x;
        double y;
        double z;
        static const Vec3 ZERO;
        static const Vec3 UNIT_X;
        static const Vec3 UNIT_Y;
        static const Vec3 UNIT_Z;

        // Constructors
        Vec3();
        Vec3(double x, double y, double z);

        // Conversions
        [[nodiscard]] Vec2 vec2() const;
        [[nodiscard]] Vec4 vec4(double w) const;

        // Operations
        /// @return A copy of the vector itself.
        Vec3 operator+() const;
        /// @return The reversed vector.
        Vec3 operator-() const;
        /// @return Result of adding this and other.
        Vec3 operator+(const Vec3 &other) const;
        /// @return Result of subtracting other from this.
        Vec3 operator-(const Vec3 &other) const;
        /// @return Add other to this vector.
        Vec3& operator+=(const Vec3 &other);
        /// @return Subtract other from this vector.
        Vec3& operator-=(const Vec3 &other);

        /// @return Dot product of this and other.
        [[nodiscard]] double dot(const Vec3 &other) const;
        /// @return Cross product of this and other.
        [[nodiscard]] Vec3 cross(const Vec3 &other) const;
        /// @return Element-wise multiplication of this and other.
        Vec3 operator*(const Vec3 &other) const;
        /// @return Results of multiplying by a scalar.
        [[nodiscard]] Vec3 mult(double factor) const;

        /// @return 2-norm of the vector.
        [[nodiscard]] double norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec3 normalize() const;
    };
}

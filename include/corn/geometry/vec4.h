#pragma once

namespace corn {
    struct Vec2;
    struct Vec3;

    /**
     * @class Vec3
     * @brief 3D vector class.
     */
    struct Vec4 {
        double x;
        double y;
        double z;
        double w;
        static const Vec4 ZERO;
        static const Vec4 UNIT_X;
        static const Vec4 UNIT_Y;
        static const Vec4 UNIT_Z;
        static const Vec4 UNIT_W;

        // Constructors
        Vec4();
        Vec4(double x, double y, double z, double w);

        // Conversions
        [[nodiscard]] Vec2 vec2() const;
        [[nodiscard]] Vec3 vec3() const;

        // Operations
        /// @return A copy of the vector itself.
        Vec4 operator+() const;
        /// @return The reversed vector.
        Vec4 operator-() const;
        /// @return Result of adding this and other.
        Vec4 operator+(const Vec4 &other) const;
        /// @return Result of subtracting other from this.
        Vec4 operator-(const Vec4 &other) const;
        /// @return Add other to this vector.
        Vec4& operator+=(const Vec4 &other);
        /// @return Subtract other from this vector.
        Vec4& operator-=(const Vec4 &other);

        /// @return Dot product of this and other.
        [[nodiscard]] double dot(const Vec4 &other) const;
        /// @return Element-wise multiplication of this and other.
        Vec4 operator*(const Vec4 &other) const;
        /// @return Results of multiplying by a scalar.
        [[nodiscard]] Vec4 mult(double factor) const;

        /// @return 2-norm of the vector.
        [[nodiscard]] double norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec4 normalize() const;
    };

    inline const Vec4 Vec4::ZERO = {0.0, 0.0, 0.0, 0.0};
    inline const Vec4 Vec4::UNIT_X = {1.0, 0.0, 0.0, 0.0};
    inline const Vec4 Vec4::UNIT_Y = {0.0, 1.0, 0.0, 0.0};
    inline const Vec4 Vec4::UNIT_Z = {0.0, 0.0, 1.0, 0.0};
    inline const Vec4 Vec4::UNIT_W = {0.0, 0.0, 0.0, 1.0};
}

#pragma once

namespace corn {
    struct Vec3;
    struct Vec4;

    /**
     * @class Vec2
     * @brief 2D vector class.
     */
    struct Vec2 {
        double x, y;
        static const Vec2 ZERO, UNIT_X, UNIT_Y;

        // Constructors
        Vec2();
        Vec2(double x, double y);

        // Conversions
        [[nodiscard]] Vec3 vec3(double z) const;
        [[nodiscard]] Vec4 vec4(double z, double w) const;

        // Operations
        /// @return A copy of the vector itself.
        Vec2 operator+() const;
        /// @return The reversed vector.
        Vec2 operator-() const;
        /// @return Result of adding this and other.
        Vec2 operator+(const Vec2& other) const;
        /// @return Result of subtracting other from this.
        Vec2 operator-(const Vec2& other) const;
        /**
         * @brief Add other to this vector in-place.
         * @return Reference to itself.
         */
        Vec2& operator+=(const Vec2& other);
        /**
         * @brief Subtract other from this vector in-place.
         * @return Reference to itself.
         */
        Vec2& operator-=(const Vec2& other);

        /// @return Dot product of this and other.
        [[nodiscard]] double dot(const Vec2& other) const;
        /// @return Element-wise multiplication of this and other.
        Vec2 operator*(const Vec2& other) const;
        /// @return Results of multiplying by a scalar.
        [[nodiscard]] Vec2 mult(double factor) const;

        /// @return 2-norm of the vector.
        [[nodiscard]] double norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec2 normalize() const;
    };

    inline const Vec2 Vec2::ZERO = {0.0, 0.0};
    inline const Vec2 Vec2::UNIT_X = {1.0, 0.0};
    inline const Vec2 Vec2::UNIT_Y = {0.0, 1.0};
}

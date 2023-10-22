#pragma once

namespace corn {
    struct Vec3;
    struct Vec4;

    /**
     * @class Vec2
     * @brief 2D vector class.
     */
    struct Vec2 {
        float x, y;

        // Constructors
        Vec2();
        Vec2(float x, float y);

        // Constants
        static const Vec2& ZERO();
        static const Vec2& UNIT_X();
        static const Vec2& UNIT_Y();

        // Conversions
        [[nodiscard]] Vec3 vec3(float z) const;
        [[nodiscard]] Vec4 vec4(float z, float w) const;

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
        [[nodiscard]] float dot(const Vec2& other) const;
        /// @return Element-wise multiplication of this and other.
        Vec2 operator*(const Vec2& other) const;
        /**
         * @brief Element-wise in-place multiplication of this and other.
         * @return Reference to itself.
         */
        Vec2 operator*=(const Vec2& other);
        /// @return Results of multiplying by a scalar.
        [[nodiscard]] Vec2 mult(float factor) const;

        /// @return 2-norm of the vector.
        [[nodiscard]] float norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec2 normalize() const;
    };
}

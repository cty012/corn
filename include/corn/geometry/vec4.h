#pragma once

namespace corn {
    struct Vec2;
    struct Vec3;

    /**
     * @class Vec4
     * @brief 4D vector class.
     */
    struct Vec4 {
        float x, y, z, w;

        // Constructors
        Vec4();
        Vec4(float x, float y, float z, float w);

        // Constants
        static const Vec4& ZERO();
        static const Vec4& UNIT_X();
        static const Vec4& UNIT_Y();
        static const Vec4& UNIT_Z();
        static const Vec4& UNIT_W();

        // Conversions
        [[nodiscard]] Vec2 vec2() const;
        [[nodiscard]] Vec3 vec3() const;

        // Operations
        /// @return A copy of the vector itself.
        Vec4 operator+() const;
        /// @return The reversed vector.
        Vec4 operator-() const;
        /// @return Result of adding this and other.
        Vec4 operator+(const Vec4& other) const;
        /// @return Result of subtracting other from this.
        Vec4 operator-(const Vec4& other) const;
        /**
         * @brief Add other to this vector in-place.
         * @return Reference to itself.
         */
        Vec4& operator+=(const Vec4& other);
        /**
         * @brief Subtract other from this vector in-place.
         * @return Reference to itself.
         */
        Vec4& operator-=(const Vec4& other);

        /// @return Dot product of this and other.
        [[nodiscard]] float dot(const Vec4& other) const;
        /// @return Element-wise multiplication of this and other.
        Vec4 operator*(const Vec4& other) const;
        /**
         * @brief Element-wise in-place multiplication of this and other.
         * @return Reference to itself.
         */
        Vec4 operator*=(const Vec4& other);
        /// @return Results of multiplying by a scalar.
        [[nodiscard]] Vec4 mult(float factor) const;

        /// @return 2-norm of the vector.
        [[nodiscard]] float norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec4 normalize() const;
    };
}

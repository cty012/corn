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
        static const Vec4 ZERO, UNIT_X, UNIT_Y, UNIT_Z, UNIT_W;

        // Constructors
        Vec4();
        Vec4(float x, float y, float z, float w);

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

    inline const Vec4 Vec4::ZERO = {0.0, 0.0, 0.0, 0.0};
    inline const Vec4 Vec4::UNIT_X = {1.0, 0.0, 0.0, 0.0};
    inline const Vec4 Vec4::UNIT_Y = {0.0, 1.0, 0.0, 0.0};
    inline const Vec4 Vec4::UNIT_Z = {0.0, 0.0, 1.0, 0.0};
    inline const Vec4 Vec4::UNIT_W = {0.0, 0.0, 0.0, 1.0};
}

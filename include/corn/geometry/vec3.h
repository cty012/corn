#pragma once

namespace corn {
    struct Vec2;
    struct Vec4;

    /**
     * @class Vec3
     * @brief 3D vector class.
     */
    struct Vec3 {
        float x, y, z;

        // Constructors
        Vec3();
        Vec3(float x, float y, float z);

        // Constants
        static const Vec3& ZERO();
        static const Vec3& UNIT_X();
        static const Vec3& UNIT_Y();
        static const Vec3& UNIT_Z();

        // Conversions
        [[nodiscard]] Vec2 vec2() const;
        [[nodiscard]] Vec4 vec4(float w) const;

        // Operations
        /// @return A copy of the vector itself.
        Vec3 operator+() const;
        /// @return The reversed vector.
        Vec3 operator-() const;
        /// @return Result of adding this and other.
        Vec3 operator+(const Vec3& other) const;
        /// @return Result of subtracting other from this.
        Vec3 operator-(const Vec3& other) const;
        /**
         * @brief Add other to this vector in-place.
         * @return Reference to itself.
         */
        Vec3& operator+=(const Vec3& other);
        /**
         * @brief Subtract other from this vector in-place.
         * @return Reference to itself.
         */
        Vec3& operator-=(const Vec3& other);

        /// @return Dot product of this and other.
        [[nodiscard]] float dot(const Vec3& other) const;
        /// @return Cross product of this and other.
        [[nodiscard]] Vec3 cross(const Vec3& other) const;
        /// @return Element-wise multiplication of this and other.
        Vec3 operator*(const Vec3& other) const;
        /**
         * @brief Element-wise in-place multiplication of this and other.
         * @return Reference to itself.
         */
        Vec3 operator*=(const Vec3& other);
        /// @return Results of multiplying by a scalar.
        [[nodiscard]] Vec3 mult(float factor) const;

        /// @return 2-norm of the vector.
        [[nodiscard]] float norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec3 normalize() const;
    };
}

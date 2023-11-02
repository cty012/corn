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

        /// @return 2-norm of the vector.
        [[nodiscard]] float norm() const;

        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec3 normalize() const;
    };

    // Operations
    /// @return A copy of the vector itself.
    Vec3 operator+(const Vec3& rhs);

    /// @return The additive inverse of the vector.
    Vec3 operator-(const Vec3& rhs);

    /// @return Result of adding lhs and rhs.
    Vec3 operator+(const Vec3& lhs, const Vec3& rhs);

    /// @return Result of subtracting rhs from lhs.
    Vec3 operator-(const Vec3& lhs, const Vec3& rhs);

    /// @return Element-wise multiplication of lhs and rhs.
    Vec3 operator*(const Vec3& lhs, const Vec3& rhs);

    /// @return Result of multiplying by a scalar.
    Vec3 operator*(const Vec3& vec, float scalar);

    /// @return Result of multiplying by a scalar.
    Vec3 operator*(float scalar, const Vec3& vec);

    /**
     * @brief Add rhs to lhs vector in-place.
     * @return Reference to itself.
     */
    Vec3& operator+=(Vec3& lhs, const Vec3& rhs);

    /**
     * @brief Subtract rhs from lhs vector in-place.
     * @return Reference to itself.
     */
    Vec3& operator-=(Vec3& lhs, const Vec3& rhs);

    /**
     * @brief Element-wise in-place multiplication of lhs and rhs.
     * @return Reference to itself.
     */
    Vec3& operator*=(Vec3& lhs, const Vec3& rhs);
}

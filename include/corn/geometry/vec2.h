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

        /// @return 2-norm of the vector.
        [[nodiscard]] float norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec2 normalize() const;
    };

    // Operations
    /// @return A copy of the vector itself.
    Vec2 operator+(const Vec2& rhs);

    /// @return The additive inverse of the vector.
    Vec2 operator-(const Vec2& rhs);

    /// @return Result of adding lhs and rhs.
    Vec2 operator+(const Vec2& lhs, const Vec2& rhs);

    /// @return Result of subtracting rhs from lhs.
    Vec2 operator-(const Vec2& lhs, const Vec2& rhs);

    /// @return Element-wise multiplication of lhs and rhs.
    Vec2 operator*(const Vec2& lhs, const Vec2& rhs);

    /// @return Result of multiplying by a scalar.
    Vec2 operator*(const Vec2& vec, float scalar);

    /// @return Result of multiplying by a scalar.
    Vec2 operator*(float scalar, const Vec2& vec);

    /**
     * @brief Add rhs to lhs vector in-place.
     * @return Reference to lhs.
     */
    Vec2& operator+=(Vec2& lhs, const Vec2& rhs);

    /**
     * @brief Subtract rhs from lhs vector in-place.
     * @return Reference to lhs.
     */
    Vec2& operator-=(Vec2& lhs, const Vec2& rhs);

    /**
     * @brief Element-wise in-place multiplication of lhs and rhs.
     * @return Reference to lhs.
     */
    Vec2& operator*=(Vec2& lhs, const Vec2& rhs);
}

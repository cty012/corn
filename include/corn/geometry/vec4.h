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

        /// @return 2-norm of the vector.
        [[nodiscard]] float norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec4 normalize() const;
    };

    // Operations
    /// @return A copy of the vector itself.
    Vec4 operator+(const Vec4& rhs);

    /// @return The additive inverse of the vector.
    Vec4 operator-(const Vec4& rhs);

    /// @return Result of adding lhs and rhs.
    Vec4 operator+(const Vec4& lhs, const Vec4& rhs);

    /// @return Result of subtracting rhs from lhs.
    Vec4 operator-(const Vec4& lhs, const Vec4& rhs);

    /// @return Element-wise multiplication of lhs and rhs.
    Vec4 operator*(const Vec4& lhs, const Vec4& rhs);

    /// @return Result of multiplying by a scalar.
    Vec4 operator*(const Vec4& vec, float scalar);

    /// @return Result of multiplying by a scalar.
    Vec4 operator*(float scalar, const Vec4& vec);

    /**
     * @brief Add rhs to lhs vector in-place.
     * @return Reference to lhs.
     */
    Vec4& operator+=(Vec4& lhs, const Vec4& rhs);

    /**
     * @brief Subtract rhs from lhs vector in-place.
     * @return Reference to lhs.
     */
    Vec4& operator-=(Vec4& lhs, const Vec4& rhs);

    /**
     * @brief Element-wise in-place multiplication of lhs and rhs.
     * @return Reference to lhs.
     */
    Vec4& operator*=(Vec4& lhs, const Vec4& rhs);
}

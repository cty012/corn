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

        /// @brief Constructor.
        Vec4() noexcept;

        /// @brief Constructor.
        Vec4(float x, float y, float z, float w) noexcept;

        /// @return Zero vector.
        [[nodiscard]] static const Vec4& ZERO() noexcept;

        /// @return Unit vector in the X direction.
        [[nodiscard]] static const Vec4& UNIT_X() noexcept;

        /// @return Unit vector in the Y direction.
        [[nodiscard]] static const Vec4& UNIT_Y() noexcept;

        /// @return Unit vector in the Z direction.
        [[nodiscard]] static const Vec4& UNIT_Z() noexcept;

        /// @return Unit vector in the W direction.
        [[nodiscard]] static const Vec4& UNIT_W() noexcept;

        /// @return Vector obtained by dropping the fourth dimension.
        [[nodiscard]] Vec2 vec2() const noexcept;

        /// @return Vector obtained by dropping the third and fourth dimensions.
        [[nodiscard]] Vec3 vec3() const noexcept;

        /// @return 2-norm of the vector.
        [[nodiscard]] float norm() const noexcept;

        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec4 normalize() const noexcept;
    };

    // Operations
    /// @return A copy of the vector itself.
    [[nodiscard]] Vec4 operator+(const Vec4& rhs) noexcept;

    /// @return The additive inverse of the vector.
    [[nodiscard]] Vec4 operator-(const Vec4& rhs) noexcept;

    /// @return Result of adding lhs and rhs.
    [[nodiscard]] Vec4 operator+(const Vec4& lhs, const Vec4& rhs) noexcept;

    /// @return Result of subtracting rhs from lhs.
    [[nodiscard]] Vec4 operator-(const Vec4& lhs, const Vec4& rhs) noexcept;

    /// @return Element-wise multiplication of lhs and rhs.
    [[nodiscard]] Vec4 operator*(const Vec4& lhs, const Vec4& rhs) noexcept;

    /// @return Result of multiplying by a scalar.
    [[nodiscard]] Vec4 operator*(const Vec4& vec, float scalar) noexcept;

    /// @return Result of multiplying by a scalar.
    [[nodiscard]] Vec4 operator*(float scalar, const Vec4& vec) noexcept;

    /**
     * @brief Add rhs to lhs vector in-place.
     * @return Reference to lhs.
     */
    Vec4& operator+=(Vec4& lhs, const Vec4& rhs) noexcept;

    /**
     * @brief Subtract rhs from lhs vector in-place.
     * @return Reference to lhs.
     */
    Vec4& operator-=(Vec4& lhs, const Vec4& rhs) noexcept;

    /**
     * @brief In-place multiplication of vec and scalar.
     * @return Reference to vec.
     */
    Vec4& operator*=(Vec4& vec, float scalar) noexcept;

    /**
     * @brief Element-wise in-place multiplication of lhs and rhs.
     * @return Reference to lhs.
     */
    Vec4& operator*=(Vec4& lhs, const Vec4& rhs) noexcept;
}

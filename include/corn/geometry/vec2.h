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

        /// @brief Constructor.
        Vec2() noexcept;

        /// @brief Constructor.
        Vec2(float x, float y) noexcept;

        /// @return Zero vector.
        [[nodiscard]] static const Vec2& ZERO() noexcept;

        /// @return Unit vector in the X direction.
        [[nodiscard]] static const Vec2& UNIT_X() noexcept;

        /// @return Unit vector in the Y direction.
        [[nodiscard]] static const Vec2& UNIT_Y() noexcept;

        /**
         * @param z Value of the third dimension.
         * @return Vector attached with the third dimension.
         */
        [[nodiscard]] Vec3 vec3(float z) const noexcept;

        /**
         * @param z Value of the third dimension.
         * @param w Value of the fourth dimension.
         * @return Vector attached with the third and fourth dimensions.
         */
        [[nodiscard]] Vec4 vec4(float z, float w) const noexcept;

        /// @return 2-norm of the vector.
        [[nodiscard]] float norm() const noexcept;

        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec2 normalize() const noexcept;
    };

    // Operations
    /// @return A copy of the vector itself.
    [[nodiscard]] Vec2 operator+(const Vec2& rhs) noexcept;

    /// @return The additive inverse of the vector.
    [[nodiscard]] Vec2 operator-(const Vec2& rhs) noexcept;

    /// @return Result of adding lhs and rhs.
    [[nodiscard]] Vec2 operator+(const Vec2& lhs, const Vec2& rhs) noexcept;

    /// @return Result of subtracting rhs from lhs.
    [[nodiscard]] Vec2 operator-(const Vec2& lhs, const Vec2& rhs) noexcept;

    /// @return Element-wise multiplication of lhs and rhs.
    [[nodiscard]] Vec2 operator*(const Vec2& lhs, const Vec2& rhs) noexcept;

    /// @return Result of multiplying by a scalar.
    [[nodiscard]] Vec2 operator*(const Vec2& vec, float scalar) noexcept;

    /// @return Result of multiplying by a scalar.
    [[nodiscard]] Vec2 operator*(float scalar, const Vec2& vec) noexcept;

    /**
     * @brief Add rhs to lhs vector in-place.
     * @return Reference to lhs.
     */
    Vec2& operator+=(Vec2& lhs, const Vec2& rhs) noexcept;

    /**
     * @brief Subtract rhs from lhs vector in-place.
     * @return Reference to lhs.
     */
    Vec2& operator-=(Vec2& lhs, const Vec2& rhs) noexcept;

    /**
     * @brief In-place multiplication of vec and scalar.
     * @return Reference to vec.
     */
    Vec2& operator*=(Vec2& vec, float scalar) noexcept;

    /**
     * @brief Element-wise in-place multiplication of lhs and rhs.
     * @return Reference to lhs.
     */
    Vec2& operator*=(Vec2& lhs, const Vec2& rhs) noexcept;
}

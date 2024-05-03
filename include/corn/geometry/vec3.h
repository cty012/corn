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

        /// @brief Constructor.
        Vec3() noexcept;

        /// @brief Constructor.
        Vec3(float x, float y, float z) noexcept;

        /// @return Zero vector.
        [[nodiscard]] static const Vec3& ZERO() noexcept;

        /// @return Unit vector in the X direction.
        [[nodiscard]] static const Vec3& UNIT_X() noexcept;

        /// @return Unit vector in the Y direction.
        [[nodiscard]] static const Vec3& UNIT_Y() noexcept;

        /// @return Unit vector in the Z direction.
        [[nodiscard]] static const Vec3& UNIT_Z() noexcept;

        /// @return Vector obtained by dropping the third dimension.
        [[nodiscard]] Vec2 vec2() const noexcept;

        /**
         * @param z Value of the fourth dimension.
         * @return Vector attached with the fourth dimension.
         */
        [[nodiscard]] Vec4 vec4(float w) const noexcept;

        /// @return 2-norm of the vector.
        [[nodiscard]] float norm() const noexcept;

        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec3 normalize() const noexcept;
    };

    // Operations
    /// @return A copy of the vector itself.
    [[nodiscard]] Vec3 operator+(const Vec3& rhs) noexcept;

    /// @return The additive inverse of the vector.
    [[nodiscard]] Vec3 operator-(const Vec3& rhs) noexcept;

    /// @return Result of adding lhs and rhs.
    [[nodiscard]] Vec3 operator+(const Vec3& lhs, const Vec3& rhs) noexcept;

    /// @return Result of subtracting rhs from lhs.
    [[nodiscard]] Vec3 operator-(const Vec3& lhs, const Vec3& rhs) noexcept;

    /// @return Element-wise multiplication of lhs and rhs.
    [[nodiscard]] Vec3 operator*(const Vec3& lhs, const Vec3& rhs) noexcept;

    /// @return Result of multiplying by a scalar.
    [[nodiscard]] Vec3 operator*(const Vec3& vec, float scalar) noexcept;

    /// @return Result of multiplying by a scalar.
    [[nodiscard]] Vec3 operator*(float scalar, const Vec3& vec) noexcept;

    /**
     * @brief Add rhs to lhs vector in-place.
     * @return Reference to lhs.
     */
    Vec3& operator+=(Vec3& lhs, const Vec3& rhs) noexcept;

    /**
     * @brief Subtract rhs from lhs vector in-place.
     * @return Reference to lhs.
     */
    Vec3& operator-=(Vec3& lhs, const Vec3& rhs) noexcept;

    /**
     * @brief In-place multiplication of vec and scalar.
     * @return Reference to vec.
     */
    Vec3& operator*=(Vec3& vec, float scalar) noexcept;

    /**
     * @brief Element-wise in-place multiplication of lhs and rhs.
     * @return Reference to lhs.
     */
    Vec3& operator*=(Vec3& lhs, const Vec3& rhs) noexcept;
}

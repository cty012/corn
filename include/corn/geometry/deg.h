#pragma once

namespace corn {
    /**
     * @class Deg
     * @brief Degree represented by a float in range [0, 360).
     */
    struct Deg {
        Deg(float val = 0.0) noexcept;  // NOLINT

        /// @brief Getter of the float value.
        [[nodiscard]] float get() const noexcept;

        /// @brief Setter of the float value.
        void set(float val) noexcept;

        /// @return Sine of the degree.
        [[nodiscard]] float sin() const noexcept;

        /// @return Cosine of the degree.
        [[nodiscard]] float cos() const noexcept;

    private:
        float val_;
    };

    // Operations
    /// @return A copy of the degree itself.
    [[nodiscard]] Deg operator+(const Deg& rhs) noexcept;

    /// @return The additive inverse of the degree.
    [[nodiscard]] Deg operator-(const Deg& rhs) noexcept;

    /// @return Result of adding lhs and rhs.
    [[nodiscard]] Deg operator+(const Deg& lhs, const Deg& rhs) noexcept;

    /// @return Result of subtracting rhs from lhs.
    [[nodiscard]] Deg operator-(const Deg& lhs, const Deg& rhs) noexcept;

    /// @return Result of multiplying by a scalar.
    [[nodiscard]] Deg operator*(const Deg& deg, float scalar) noexcept;

    /// @return Result of multiplying by a scalar.
    [[nodiscard]] Deg operator*(float scalar, const Deg& deg) noexcept;

    /**
     * @brief Add rhs to lhs degree in-place.
     * @return Reference to lhs.
     */
    Deg& operator+=(Deg& lhs, const Deg& rhs) noexcept;

    /**
     * @brief Subtract rhs from lhs degree in-place.
     * @return Reference to lhs.
     */
    Deg& operator-=(Deg& lhs, const Deg& rhs) noexcept;
}

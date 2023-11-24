#pragma once

#include <corn/geometry/vec2.h>

namespace corn {
    /**
     * @class Deg
     * @brief Degree represented by a float in range [0, 360).
     */
    struct Deg {
        Deg(float val = 0.0);  // NOLINT

        /// @brief Getter of the float value.
        [[nodiscard]] float get() const;
        /// @brief Setter of the float value.
        void set(float val);

        /// @return Sine of the degree.
        [[nodiscard]] float sin() const;
        /// @return Cosine of the degree.
        [[nodiscard]] float cos() const;

    private:
        float val_;
    };

    // Operations
    /// @return A copy of the degree itself.
    Deg operator+(const Deg& rhs);

    /// @return The additive inverse of the degree.
    Deg operator-(const Deg& rhs);

    /// @return Result of adding lhs and rhs.
    Deg operator+(const Deg& lhs, const Deg& rhs);

    /// @return Result of subtracting rhs from lhs.
    Deg operator-(const Deg& lhs, const Deg& rhs);

    /// @return Result of multiplying by a scalar.
    Deg operator*(const Deg& deg, float scalar);

    /// @return Result of multiplying by a scalar.
    Deg operator*(float scalar, const Deg& deg);

    /**
     * @brief Add rhs to lhs degree in-place.
     * @return Reference to lhs.
     */
    Deg& operator+=(Deg& lhs, const Deg& rhs);

    /**
     * @brief Subtract rhs from lhs degree in-place.
     * @return Reference to lhs.
     */
    Deg& operator-=(Deg& lhs, const Deg& rhs);
}

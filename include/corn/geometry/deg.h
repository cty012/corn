#pragma once

#include <corn/geometry/vec2.h>

namespace corn {
    /**
     * @class Deg
     * @brief float in range [0, 360).
     *
     * @todo Implement this struct.
     */
    struct Deg {
        Deg(float val = 0.0);  // NOLINT

        [[nodiscard]] float get() const;
        void set(float val);

        /// @return A copy of the degree itself.
        Deg operator+() const;
        /// @return The reversed degree.
        Deg operator-() const;
        /// @return Result of adding this and other.
        Deg operator+(const Deg& other) const;
        /// @return Result of subtracting other from this.
        Deg operator-(const Deg& other) const;
        /**
         * @brief Add other to this degree in-place.
         * @return Reference to itself.
         */
        Deg& operator+=(const Deg& other);
        /**
         * @brief Subtract other from this degree in-place.
         * @return Reference to itself.
         */
        Deg& operator-=(const Deg& other);

        /// @return Results of multiplying by a scalar.
        [[nodiscard]] Deg mult(float factor) const;
        /// @return Sine of the degree.
        [[nodiscard]] float sin() const;
        /// @return Cosine of the degree.
        [[nodiscard]] float cos() const;
        /// @brief Rotate a 2D point.
        [[nodiscard]] Vec2 rotate(const Vec2& point) const;

    private:
        float _val;
    };
}

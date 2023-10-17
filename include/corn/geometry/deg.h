#pragma once

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

        // TODO: operator overload

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

    private:
        float _val;
    };
}

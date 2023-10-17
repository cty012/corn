#pragma once

namespace corn {
    /**
     * @class Deg
     * @brief double in range [0, 360).
     *
     * @todo Implement this struct.
     */
    struct Deg {
        Deg(double val = 0.0);  // NOLINT

        [[nodiscard]] double get() const;
        void set(double val);

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
        [[nodiscard]] Deg mult(double factor) const;
        /// @return Sine of the degree.
        [[nodiscard]] double sin() const;
        /// @return Cosine of the degree.
        [[nodiscard]] double cos() const;

    private:
        double _val;
    };
}

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace corn {
    template <typename T>
    concept Numeric = std::is_arithmetic_v<T>;

    // Mixin for x, y access (N == 2)
    template <typename T>
    struct HasXY {
        T& x;
        T& y;

        explicit HasXY(std::array<T, 2>& data) : x(data[0]), y(data[1]) {}
    };

    // Mixin for x, y, z access (N == 3)
    template <typename T>
    struct HasXYZ {
        T& x;
        T& y;
        T& z;

        explicit HasXYZ(std::array<T, 3>& data) : x(data[0]), y(data[1]), z(data[2]) {}
    };

    // Mixin for x, y, z, w access (N == 4)
    template <typename T>
    requires(Numeric<T>)
    struct HasXYZW {
        T& x;
        T& y;
        T& z;
        T& w;

        explicit HasXYZW(std::array<T, 4>& data) : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}
    };

    // Mixin for other dimensions (N = 1 or N > 4)
    struct HasNoXYZW {
        template <typename T, int N>
        requires(Numeric<T> && (N == 1 || N > 4))
        explicit HasNoXYZW(std::array<T, N>&) noexcept {}
    };

    // Select correct mixin
    template <typename T, int N>
    using VecMixin = std::conditional_t<
            N == 2, HasXY<T>,
            std::conditional_t<
                    N == 3, HasXYZ<T>,
                            std::conditional_t<
                                N == 4, HasXYZW<T>,
                                HasNoXYZW>>>;

    /**
     * @class Vec
     * @brief Vector class.
     * @tparam T Numeric type. Can be any arithmetic type.
     * @tparam N Dimension of the vector. Must be a positive integer.
     */
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    struct Vec : VecMixin<T, N> {
        /// @brief Constructor.
        Vec() : VecMixin<T, N>(this->data_), data_{} {}

        /// @brief Constructor.
        template <typename... Args>
        requires(sizeof...(Args) == N && std::conjunction_v<std::is_convertible<Args, T>...>)
        explicit Vec(Args... args) noexcept
                : VecMixin<T, N>(this->data_), data_{{ static_cast<T>(std::forward<Args>(args))... }} {}

        /// @brief Copy Constructor.
        Vec(const Vec<T, N>& other) noexcept : VecMixin<T, N>(this->data_), data_(other.data_) {}

        /// @brief Copy Assignment.
        Vec<T, N>& operator=(const Vec<T, N>& other) noexcept {
            if (this == &other) return *this;
            this->data_ = other.data_;
            return *this;
        }

        /// @brief Move Constructor.
        Vec(Vec<T, N>&& other) noexcept : VecMixin<T, N>(this->data_), data_(std::move(other.data_)) {}

        /// @brief Move Assignment.
        Vec<T, N>& operator=(Vec<T, N>&& other) noexcept {
            if (this == &other) return *this;
            this->data_ = std::move(other.data_);
            return *this;
        }

        /// @brief Zero vector.
        static const Vec& ZERO() noexcept {
            static const Vec zero;
            return zero;
        }

        /// @brief Unit vector in the X direction.
        static const Vec& UNIT_X() noexcept
        requires(N >= 2 && N <= 4) {
            static const Vec unitX = [] {
                Vec v;
                v.x = static_cast<T>(1);
                return v;
            }();
            return unitX;
        }

        /// @brief Unit vector in the Y direction.
        static const Vec& UNIT_Y() noexcept
        requires(N >= 2 && N <= 4) {
            static const Vec unitY = [] {
                Vec v;
                v.y = static_cast<T>(1);
                return v;
            }();
            return unitY;
        }

        /// @brief Unit vector in the Z direction.
        static const Vec& UNIT_Z() noexcept
        requires(N >= 3 && N <= 4) {
            static const Vec unitZ = [] {
                Vec v;
                v.z = static_cast<T>(1);
                return v;
            }();
            return unitZ;
        }

        /// @brief Unit vector in the W direction.
        static const Vec& UNIT_W() noexcept
        requires(N == 4) {
            static const Vec unitW = [] {
                Vec v;
                v.w = static_cast<T>(1);
                return v;
            }();
            return unitW;
        }

        /**
         * @brief Get the dimension of the vector.
         * @return Dimension of the vector.
         */
        [[nodiscard]] static constexpr int dim() noexcept {
            return N;
        }

        /**
         * @param index Index of the element to access.
         * @return Reference to the element at the given index.
         */
        [[nodiscard]] T& operator[](int index) noexcept {
            return this->data_[index];
        }

        /**
         * @param index Index of the element to access.
         * @return Reference to the element at the given index.
         */
        [[nodiscard]] const T& operator[](int index) const noexcept {
            return this->data_[index];
        }

        /**
         * @param index Index of the element to access.
         * @return Reference to the element at the given index.
         */
        [[nodiscard]] T& at(int index) noexcept {
            return this->data_.at(index);
        }

        /**
         * @param index Index of the element to access.
         * @return Reference to the element at the given index.
         */
        [[nodiscard]] const T& at(int index) const noexcept {
            return this->data_.at(index);
        }

        /**
         * @brief Convert to a vector of another type or dimension.
         * @tparam U Type to convert to.
         * @tparam M Dimension of the new vector.
         * @return Converted vector.
         */
        template <typename U, int M>
        requires(Numeric<U> && M > 0)
        [[nodiscard]] Vec<U, M> to() const noexcept {
            Vec<U, M> result;
            for (int i = 0; i < std::min(N, M); i++) {
                result[i] = static_cast<U>(this->data_[i]);
            }
            return result;
        }

        /**
         * @brief Convert to a vector of another type and same dimension.
         * @tparam U Type to convert to.
         * @return Converted vector.
         */
        template <typename U>
        requires(Numeric<U>)
        [[nodiscard]] Vec<U, N> to() const noexcept {
            return this->to<U, N>();
        }

        /**
         * @brief Convert to a vector of the same type and another dimension.
         * @tparam M Dimension of the new vector.
         * @return Converted vector.
         */
        template <int M>
        requires(M > 0)
        [[nodiscard]] Vec<T, M> to() const noexcept {
            return this->to<T, M>();
        }

        template <int M, typename... Args>
        requires(M > 0 && sizeof...(Args) == M && std::conjunction_v<std::is_convertible<Args, T>...>)
        [[nodiscard]] Vec<T, N + M> append(Args... args) const noexcept {
            Vec<T, N + M> result;
            T appended[] = { static_cast<T>(args)... };
            for (int i = 0; i < N; i++) {
                result[i] = this->data_[i];
            }
            for (int i = 0; i < M; i++) {
                result[N + i] = appended[i];
            }
            return result;
        }

        [[nodiscard]] T norm() const noexcept {
            T sum = 0;
            for (const T& val : this->data_) {
                sum += val * val;
            }
            return std::sqrt(sum);
        }

        [[nodiscard]] Vec normalize() const noexcept
        requires std::is_floating_point_v<T> {
            T n = this->norm();
            return n == 0 ? *this : *this * (1 / n);
        }

    private:
        std::array<T, N> data_;
    };

    // Operators
    /**
     * @param lhs First vector.
     * @param rhs Second vector.
     * @return A vector formed by concatenating the two vectors.
     */
    template <typename T, int N, int M>
    requires(Numeric<T> && N > 0 && M > 0)
    Vec<T, N + M> operator|(const Vec<T, N>& lhs, const Vec<T, M>& rhs) noexcept {
        Vec<T, N + M> result;
        for (int i = 0; i < N; i++) {
            result[i] = lhs[i];
        }
        for (int i = 0; i < M; i++) {
            result[N + i] = rhs[i];
        }
        return result;
    }

    /// @return A copy of the vector itself.
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    Vec<T, N> operator+(const Vec<T, N>& rhs) noexcept {
        return rhs;
    }

    /// @return The additive inverse of the vector.
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    Vec<T, N> operator-(const Vec<T, N>& rhs) noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; i++) {
            result[i] = -rhs[i];
        }
        return result;
    }

    /// @return Result of adding lhs and rhs.
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    Vec<T, N> operator+(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; i++) {
            result[i] = lhs[i] + rhs[i];
        }
        return result;
    }

    /// @return Result of subtracting rhs from lhs.
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    Vec<T, N> operator-(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; i++) {
            result[i] = lhs[i] - rhs[i];
        }
        return result;
    }

    /// @return Element-wise multiplication of lhs and rhs.
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    Vec<T, N> operator*(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; i++) {
            result[i] = lhs[i] * rhs[i];
        }
        return result;
    }

    /// @return Result of multiplying a vector and a scalar.
    template <typename T, typename U, int N>
    requires(Numeric<T> && N > 0 && std::is_convertible_v<U, T>)
    Vec<T, N> operator*(const Vec<T, N>& vec, U scalar) noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; i++) {
            result[i] = vec[i] * scalar;
        }
        return result;
    }

    /// @return Result of multiplying a scalar and a vector.
    template <typename T, typename U, int N>
    requires(Numeric<T> && N > 0 && std::is_convertible_v<U, T>)
    Vec<T, N> operator*(U scalar, const Vec<T, N>& vec) noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; i++) {
            result[i] = scalar * vec[i];
        }
        return result;
    }

    /**
     * @brief Add rhs to lhs vector in-place.
     * @param lhs First vector.
     * @param rhs Second vector.
     * @return Reference to lhs.
     */
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    Vec<T, N>& operator+=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        for (int i = 0; i < N; i++) {
            lhs[i] += rhs[i];
        }
        return lhs;
    }

    /**
     * @brief Subtract rhs from lhs vector in-place.
     * @param lhs First vector.
     * @param rhs Second vector.
     * @return Reference to lhs.
     */
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    Vec<T, N>& operator-=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        for (int i = 0; i < N; i++) {
            lhs[i] -= rhs[i];
        }
        return lhs;
    }

    /**
     * @brief In-place multiplication of vector and scalar.
     * @param vec Vector to be multiplied.
     * @param scalar Scalar to multiply with.
     * @return Reference to the vector.
     */
    template <typename T, typename U, int N>
    requires(Numeric<T> && N > 0 && std::is_convertible_v<U, T>)
    Vec<T, N>& operator*=(Vec<T, N>& vec, U scalar) noexcept {
        for (int i = 0; i < N; i++) {
            vec[i] *= scalar;
        }
        return vec;
    }

    /**
     * @brief Element-wise in-place multiplication of two vectors.
     * @param lhs First vector.
     * @param rhs Second vector.
     * @return Reference to lhs.
     */
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    Vec<T, N>& operator*=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        for (int i = 0; i < N; i++) {
            lhs[i] *= rhs[i];
        }
        return lhs;
    }

    /**
     * @brief Compare two vectors for equality.
     * @param lhs First vector.
     * @param rhs Second vector.
     * @return Whether the vectors are equal.
     */
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    bool operator==(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        for (int i = 0; i < N; i++) {
            if (lhs[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Compare two vectors for inequality.
     * @param lhs First vector.
     * @param rhs Second vector.
     * @return Whether the vectors are not equal.
     */
    template <typename T, int N>
    requires(Numeric<T> && N > 0)
    bool operator!=(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        return !(lhs == rhs);
    }

    // Aliases
    using Vec2f = Vec<float, 2>;
    using Vec3f = Vec<float, 3>;
    using Vec4f = Vec<float, 4>;

    using Vec2d = Vec<double, 2>;
    using Vec3d = Vec<double, 3>;
    using Vec4d = Vec<double, 4>;

    using Vec2i = Vec<int32_t, 2>;
    using Vec3i = Vec<int32_t, 3>;
    using Vec4i = Vec<int32_t, 4>;

    using Vec2u = Vec<uint32_t, 2>;
    using Vec3u = Vec<uint32_t, 3>;
    using Vec4u = Vec<uint32_t, 4>;

    using Vec3uc = Vec<uint8_t, 3>;
    using Vec4uc = Vec<uint8_t, 4>;
}

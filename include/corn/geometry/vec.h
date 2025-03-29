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
    class HasXY {
    public:
        T& x;
        T& y;

        explicit HasXY(std::array<T, 2>& data) : x(data[0]), y(data[1]) {}
    };

    // Mixin for x, y, z access (N == 3)
    template <typename T>
    class HasXYZ {
    public:
        T& x;
        T& y;
        T& z;

        explicit HasXYZ(std::array<T, 3>& data) : x(data[0]), y(data[1]), z(data[2]) {}
    };

    // Mixin for x, y, z, w access (N == 4)
    template <typename T>
    requires(Numeric<T>)
    class HasXYZW {
    public:
        T& x;
        T& y;
        T& z;
        T& w;

        explicit HasXYZW(std::array<T, 4>& data) : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}
    };

    // Mixin for other dimensions (N = 1 or N > 4)
    class HasNoXYZW {
    public:
        template <typename T, size_t N>
        requires(Numeric<T> && (N == 1 || N > 4))
        explicit HasNoXYZW(std::array<T, N>&) noexcept {}
    };

    // Select correct mixin
    template <typename T, size_t N>
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
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    class Vec : public VecMixin<T, N> {
    public:
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
        [[nodiscard]] static const Vec<T, N>& O() noexcept {
            static const Vec<T, N> zero;
            return zero;
        }

        /// @brief Unit vector in the given direction.
        template <size_t M>
        requires(M < N)
        [[nodiscard]] static const Vec<T, N>& E() noexcept {
            static const Vec<T, N> unit = [] {
                Vec<T, N> v;
                v[M] = static_cast<T>(1);
                return v;
            }();
            return unit;
        }

        /// @brief Unit vector in the X direction.
        [[nodiscard]] static const Vec<T, N>& E_X() noexcept
        requires(N >= 2 && N <= 4) {
            return E<0>();
        }

        /// @brief Unit vector in the Y direction.
        [[nodiscard]] static const Vec<T, N>& E_Y() noexcept
        requires(N >= 2 && N <= 4) {
            return E<1>();
        }

        /// @brief Unit vector in the Z direction.
        [[nodiscard]] static const Vec<T, N>& E_Z() noexcept
        requires(N >= 3 && N <= 4) {
            return E<2>();
        }

        /// @brief Unit vector in the W direction.
        [[nodiscard]] static const Vec<T, N>& E_W() noexcept
        requires(N == 4) {
            return E<3>();
        }

        /**
         * @brief Get the dimension of the vector.
         * @return Dimension of the vector.
         */
        [[nodiscard]] static constexpr size_t dim() noexcept {
            return N;
        }

        /**
         * @param index Index of the element to access.
         * @return Reference to the element at the given index.
         */
        [[nodiscard]] T& operator[](size_t index) noexcept {
            return this->data_[index];
        }

        /**
         * @param index Index of the element to access.
         * @return Reference to the element at the given index.
         */
        [[nodiscard]] const T& operator[](size_t index) const noexcept {
            return this->data_[index];
        }

        /**
         * @param index Index of the element to access.
         * @return Reference to the element at the given index.
         */
        [[nodiscard]] T& at(size_t index) noexcept {
            return this->data_.at(index);
        }

        /**
         * @param index Index of the element to access.
         * @return Reference to the element at the given index.
         */
        [[nodiscard]] const T& at(size_t index) const noexcept {
            return this->data_.at(index);
        }

        /**
         * @brief Convert to a vector of another type or dimension.
         * @tparam U Type to convert to.
         * @tparam M Dimension of the new vector.
         * @return Converted vector.
         */
        template <typename U, size_t M>
        requires(Numeric<U> && M > 0)
        [[nodiscard]] Vec<U, M> to() const noexcept {
            Vec<U, M> result;
            for (size_t i = 0; i < std::min(N, M); i++) {
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
        template <size_t M>
        requires(M > 0)
        [[nodiscard]] Vec<T, M> to() const noexcept {
            return this->to<T, M>();
        }

        /**
         * @tparam M Number of elements to prepend.
         * @param args Elements to prepend.
         * @return New vector with prepended elements.
         */
        template <size_t M, typename... Args>
        requires(M > 0 && sizeof...(Args) == M && std::conjunction_v<std::is_convertible<Args, T>...>)
        [[nodiscard]] Vec<T, N + M> prepend(Args... args) const noexcept {
            Vec<T, N + M> result;
            T prepended[] = { static_cast<T>(args)... };
            for (size_t i = 0; i < M; i++) {
                result[i] = prepended[i];
            }
            for (size_t i = 0; i < N; i++) {
                result[M + i] = this->data_[i];
            }
            return result;
        }

        /**
         * @tparam M Number of elements to append.
         * @param args Elements to append.
         * @return New vector with appended elements.
         */
        template <size_t M, typename... Args>
        requires(M > 0 && sizeof...(Args) == M && std::conjunction_v<std::is_convertible<Args, T>...>)
        [[nodiscard]] Vec<T, N + M> append(Args... args) const noexcept {
            Vec<T, N + M> result;
            T appended[] = { static_cast<T>(args)... };
            for (size_t i = 0; i < N; i++) {
                result[i] = this->data_[i];
            }
            for (size_t i = 0; i < M; i++) {
                result[N + i] = appended[i];
            }
            return result;
        }

        /// @return Norm of the vector.
        [[nodiscard]] float norm() const noexcept {
            T sum = 0;
            for (const T& val : this->data_) {
                sum += val * val;
            }
            return (float)std::sqrt(sum);
        }

        /// @return Squared norm of the vector.
        [[nodiscard]] T norm2() const noexcept {
            T sum = 0;
            for (const T& val : this->data_) {
                sum += val * val;
            }
            return sum;
        }

        /// @return Normalized vector.
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
    template <typename T, size_t N, size_t M>
    requires(Numeric<T> && N > 0 && M > 0)
    Vec<T, N + M> operator|(const Vec<T, N>& lhs, const Vec<T, M>& rhs) noexcept {
        Vec<T, N + M> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = lhs[i];
        }
        for (size_t i = 0; i < M; i++) {
            result[N + i] = rhs[i];
        }
        return result;
    }

    /// @return A copy of the vector itself.
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    Vec<T, N> operator+(const Vec<T, N>& rhs) noexcept {
        return rhs;
    }

    /// @return The additive inverse of the vector.
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    Vec<T, N> operator-(const Vec<T, N>& rhs) noexcept {
        Vec<T, N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = -rhs[i];
        }
        return result;
    }

    /// @return Result of adding lhs and rhs.
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    Vec<T, N> operator+(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        Vec<T, N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = lhs[i] + rhs[i];
        }
        return result;
    }

    /// @return Result of subtracting rhs from lhs.
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    Vec<T, N> operator-(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        Vec<T, N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = lhs[i] - rhs[i];
        }
        return result;
    }

    /// @return Element-wise multiplication of lhs and rhs.
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    Vec<T, N> operator*(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        Vec<T, N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = lhs[i] * rhs[i];
        }
        return result;
    }

    /// @return Result of multiplying a vector and a scalar.
    template <typename T, typename U, size_t N>
    requires(Numeric<T> && N > 0 && std::is_convertible_v<U, T>)
    Vec<T, N> operator*(const Vec<T, N>& vec, U scalar) noexcept {
        Vec<T, N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = vec[i] * scalar;
        }
        return result;
    }

    /// @return Result of multiplying a scalar and a vector.
    template <typename T, typename U, size_t N>
    requires(Numeric<T> && N > 0 && std::is_convertible_v<U, T>)
    Vec<T, N> operator*(U scalar, const Vec<T, N>& vec) noexcept {
        Vec<T, N> result;
        for (size_t i = 0; i < N; i++) {
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
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    Vec<T, N>& operator+=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        for (size_t i = 0; i < N; i++) {
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
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    Vec<T, N>& operator-=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        for (size_t i = 0; i < N; i++) {
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
    template <typename T, typename U, size_t N>
    requires(Numeric<T> && N > 0 && std::is_convertible_v<U, T>)
    Vec<T, N>& operator*=(Vec<T, N>& vec, U scalar) noexcept {
        for (size_t i = 0; i < N; i++) {
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
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    Vec<T, N>& operator*=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        for (size_t i = 0; i < N; i++) {
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
    template <typename T, size_t N>
    requires(Numeric<T> && N > 0)
    bool operator==(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
        for (size_t i = 0; i < N; i++) {
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
    template <typename T, size_t N>
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

// Decomposer
namespace std {
    template <typename T, size_t N>
    struct tuple_size<corn::Vec<T, N>> : std::integral_constant<size_t, N> {};

    template <size_t I, typename T, size_t N>
    struct tuple_element<I, corn::Vec<T, N>> {
        using type = T;
    };
}

namespace corn {
    // get<i> overloads
    template <size_t I, typename T, size_t N>
    T& get(Vec<T, N>& v) {
        static_assert(I < N);
        return v[I];
    }

    template <size_t I, typename T, size_t N>
    const T& get(const Vec<T, N>& v) {
        static_assert(I < N);
        return v[I];
    }
}

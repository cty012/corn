#pragma once

#include <array>
#include <concepts>
#include <format>
#include <vector>
#include <corn/geometry/vec.h>

namespace corn {
    // Helper functions
    void invHelper(size_t N, const std::vector<float>& mat, std::vector<float>& inv) noexcept;
    void detHelper(size_t N, const std::vector<float>& mat, float det) noexcept;
    void svdHelper(size_t M, size_t N, const std::vector<float>& mat, std::vector<float>& U, std::vector<float>& S, std::vector<float>& V) noexcept;

    template <size_t M, size_t N>
    class Mat {
    public:
        Mat() noexcept : data_() {}

        template <typename... Args>
        requires(sizeof...(Args) == M * N && std::conjunction_v<std::is_convertible<Args, float>...>)
        explicit Mat(Args... args) noexcept {
            const float temp[] = { static_cast<float>(args)... };
            size_t index = 0;
            for (size_t i = 0; i < M; i++) {
                for (size_t j = 0; j < N; j++) {
                    this->data_[i][j] = temp[index++];
                }
            }
        }

        template <typename... Args>
        requires(sizeof...(Args) == M && std::conjunction_v<std::is_same<Args, Vec<float, N>>...>)
        explicit Mat(Args... args) noexcept : data_{ std::forward<Args>(args)... } {}

        explicit Mat(const std::vector<float>& flattened) noexcept : data_() {
            size_t index = 0;
            for (size_t i = 0; i < M; i++) {
                for (size_t j = 0; j < N; j++) {
                    if (index >= flattened.size()) {
                        break;
                    }
                    this->data_[i][j] = flattened[index++];
                }
            }
        }

        [[nodiscard]] static const Mat& O() noexcept {
            static const Mat zero;
            return zero;
        }

        [[nodiscard]] static const Mat& I() noexcept
        requires(M == N) {
            static const Mat identity = [] {
                Mat mat;
                for (size_t i = 0; i < M; i++) {
                    mat.data_[i][i] = 1.0f;
                }
                return mat;
            }();
            return identity;
        }

        [[nodiscard]] static Mat diag(const Vec<float, M>& diag) noexcept
        requires(M == N) {
            Mat mat;
            for (size_t i = 0; i < M; i++) {
                mat.data_[i][i] = diag[i];
            }
            return mat;
        }

        [[nodiscard]] size_t rows() const noexcept {
            return M;
        }

        [[nodiscard]] size_t cols() const noexcept {
            return N;
        }

        [[nodiscard]] Vec<float, N>& row(size_t index) noexcept {
            return this->data_[index];
        }

        [[nodiscard]] const Vec<float, N>& row(size_t index) const noexcept {
            return this->data_[index];
        }

        [[nodiscard]] Vec<float, N> col(size_t index) const noexcept {
            Vec<float, M> column;
            for (size_t i = 0; i < M; ++i) {
                column[i] = this->data_[i][index];
            }
            return column;
        }

        [[nodiscard]] Vec<float, N>& operator[](size_t index) noexcept {
            return this->data_[index];
        }

        [[nodiscard]] const Vec<float, N>& operator[](size_t index) const noexcept {
            return this->data_[index];
        }

        [[nodiscard]] float& operator()(size_t r, size_t c) noexcept {
            return this->data_[r][c];
        }

        [[nodiscard]] const float& operator()(size_t r, size_t c) const noexcept {
            return this->data_[r][c];
        }

        [[nodiscard]] Vec<float, N>& at(size_t r) noexcept {
            return this->data_.at(r);
        }

        [[nodiscard]] const Vec<float, N>& at(size_t r) const noexcept {
            return this->data_.at(r);
        }

        [[nodiscard]] float& at(size_t r, size_t c) noexcept {
            return this->data_.at(r).at(c);
        }

        [[nodiscard]] const float& at(size_t r, size_t c) const noexcept {
            return this->data_.at(r).at(c);
        }

        [[nodiscard]] std::vector<float> flatten() const noexcept {
            std::vector<float> flattened;
            flattened.reserve(M * N);
            for (size_t i = 0; i < M; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    flattened.push_back(this->data_[i][j]);
                }
            }
            return flattened;
        }

        [[nodiscard]] Mat<N, M> T() const noexcept {
            Mat<N, M> transposed;
            for (size_t i = 0; i < M; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    transposed[j][i] = this->data_[i][j];
                }
            }
            return transposed;
        }

        [[nodiscard]] Mat<M, N> inv() const noexcept
        requires(M == N) {
            // Check if the matrix is invertible
            float det = this->det();
            if (det == 0.0f) {
                return Mat<M, N>::O(); // Return zero matrix if not invertible
            }

            if constexpr (M == 1) {
                return Mat<M, N>(1.0f / this->data_[0][0]);
            } else if constexpr (M == 2) {
                float invDet = 1.0f / det;
                return Mat<M, N>(
                         data_[1][1] * invDet,
                        -data_[0][1] * invDet,
                        -data_[1][0] * invDet,
                         data_[0][0] * invDet);
            } else if constexpr (M == 3) {
                // Calculate the inverse using the adjugate method
                float invDet = 1.0f / det;
                Mat<M, N> result;
                result[0][0] =  (data_[1][1] * data_[2][2] - data_[1][2] * data_[2][1]) * invDet;
                result[0][1] = -(data_[0][1] * data_[2][2] - data_[0][2] * data_[2][1]) * invDet;
                result[0][2] =  (data_[0][1] * data_[1][2] - data_[0][2] * data_[1][1]) * invDet;
                result[1][0] = -(data_[1][0] * data_[2][2] - data_[1][2] * data_[2][0]) * invDet;
                result[1][1] =  (data_[0][0] * data_[2][2] - data_[0][2] * data_[2][0]) * invDet;
                result[1][2] = -(data_[0][0] * data_[1][2] - data_[0][2] * data_[1][0]) * invDet;
                result[2][0] =  (data_[1][0] * data_[2][1] - data_[1][1] * data_[2][0]) * invDet;
                result[2][1] = -(data_[0][0] * data_[2][1] - data_[0][1] * data_[2][0]) * invDet;
                result[2][2] =  (data_[0][0] * data_[1][1] - data_[0][1] * data_[1][0]) * invDet;
                return result;
            } else {
                std::vector<float> flattened = this->flatten(), invFlattened;
                invHelper(M, flattened, invFlattened);
                Mat<M, N> result;
                for (size_t i = 0; i < M; ++i) {
                    for (size_t j = 0; j < N; ++j) {
                        result[i][j] = invFlattened[i * M + j];
                    }
                }
                return result;
            }
        }

        template <size_t K, size_t L>
        requires(K > 0 && L > 0)
        [[nodiscard]] Mat<K, L> to() const noexcept {
            Mat<K, L> result;
            for (size_t i = 0; i < std::min(M, K); i++) {
                for (size_t j = 0; j < std::min(N, L); j++) {
                    result[i][j] = this->data_[i][j];
                }
            }
            return result;
        }

        template <size_t K, typename... Args>
        requires(K > 0 && sizeof...(Args) == K && std::conjunction_v<std::is_same<Args, Vec<float, N>>...>)
        [[nodiscard]] Mat<M + K, N> prepend(Args... args) const noexcept {
            Mat<M + K, N> result;
            std::array<Vec<float, N>, K> prefix = { std::forward<Args>(args)... };
            for (size_t i = 0; i < K; i++) {
                result[i] = std::move(prefix[i]);
            }
            for (size_t i = 0; i < M; i++) {
                result[K + i] = this->data_[i];
            }
            return result;
        }

        template <size_t K, typename... Args>
        requires(K > 0 && sizeof...(Args) == K && std::conjunction_v<std::is_same<Args, Vec<float, N>>...>)
        [[nodiscard]] Mat<M + K, N> append(Args... args) const noexcept {
            Mat<M + K, N> result;
            std::array<Vec<float, N>, K> suffix = { std::forward<Args>(args)... };
            for (size_t i = 0; i < M; i++) {
                result[i] = this->data_[i];
            }
            for (size_t i = 0; i < K; i++) {
                result[M + i] = std::move(suffix[i]);
            }
            return result;
        }

        [[nodiscard]] float det() const noexcept
        requires(M == N) {
            if constexpr (M == 1) {
                return this->data_[0][0];
            } else if constexpr (M == 2) {
                return this->data_[0][0] * this->data_[1][1] - this->data_[0][1] * this->data_[1][0];
            } else if constexpr (M == 3) {
                return
                        + data_[0][0] * (data_[1][1] * data_[2][2] - data_[1][2] * data_[2][1])
                        - data_[0][1] * (data_[1][0] * data_[2][2] - data_[1][2] * data_[2][0])
                        + data_[0][2] * (data_[1][0] * data_[2][1] - data_[1][1] * data_[2][0]);
            } else if constexpr (M == 4) {
                return
                        + data_[0][0] * (
                                + data_[1][1] * (data_[2][2] * data_[3][3] - data_[2][3] * data_[3][2])
                                - data_[1][2] * (data_[2][1] * data_[3][3] - data_[2][3] * data_[3][1])
                                + data_[1][3] * (data_[2][1] * data_[3][2] - data_[2][2] * data_[3][1]))
                        - data_[0][1] * (
                                + data_[1][0] * (data_[2][2] * data_[3][3] - data_[2][3] * data_[3][2])
                                - data_[1][2] * (data_[2][0] * data_[3][3] - data_[2][3] * data_[3][0])
                                + data_[1][3] * (data_[2][0] * data_[3][2] - data_[2][2] * data_[3][0]))
                        + data_[0][2] * (
                                + data_[1][0] * (data_[2][1] * data_[3][3] - data_[2][3] * data_[3][1])
                                - data_[1][1] * (data_[2][0] * data_[3][3] - data_[2][3] * data_[3][0])
                                + data_[1][3] * (data_[2][0] * data_[3][1] - data_[2][1] * data_[3][0]))
                        - data_[0][3] * (
                                + data_[1][0] * (data_[2][1] * data_[3][2] - data_[2][2] * data_[3][1])
                                - data_[1][1] * (data_[2][0] * data_[3][2] - data_[2][2] * data_[3][0])
                                + data_[1][2] * (data_[2][0] * data_[3][1] - data_[2][1] * data_[3][0]));
            } else {
                std::vector<float> flattened = this->flatten();
                float det;
                detHelper(N, flattened, det);
                return det;
            }
        }

        void svd(Mat<M, M>* U, Vec<float, (M < N ? M : N)>* S, Mat<N, N>* V) const noexcept {
            std::vector<float> flattened = this->flatten();
            std::vector<float> u, s, v;
            svdHelper(M, N, flattened, u, s, v);
            if (U) {
                for (size_t i = 0; i < M; i++) {
                    for (size_t j = 0; j < M; j++) {
                        (*U)[i][j] = u[i * M + j];
                    }
                }
            }
            if (S) {
                for (size_t i = 0; i < (M < N ? M : N); i++) {
                    (*S)[i] = s[i];
                }
            }
            if (V) {
                for (size_t i = 0; i < N; i++) {
                    for (size_t j = 0; j < N; j++) {
                        (*V)[i][j] = v[i * N + j];
                    }
                }
            }
        }

    private:
        std::array<Vec<float, N>, M> data_;
    };

    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Mat<M, N> operator+(const Mat<M, N>& lhs, const Mat<M, N>& rhs) noexcept {
        Mat<M, N> result;
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                result[i][j] = lhs[i][j] + rhs[i][j];
            }
        }
        return result;
    }

    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Mat<M, N> operator-(const Mat<M, N>& lhs, const Mat<M, N>& rhs) noexcept {
        Mat<M, N> result;
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                result[i][j] = lhs[i][j] - rhs[i][j];
            }
        }
        return result;
    }

    template <size_t M, size_t N, size_t K>
    requires(M > 0 && N > 0 && K > 0)
    Mat<M, K> operator*(const Mat<M, N>& lhs, const Mat<N, K>& rhs) noexcept {
        Mat<M, K> result;
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                for (size_t k = 0; k < K; k++) {
                    result[i][k] += lhs[i][j] * rhs[j][k];
                }
            }
        }
        return result;
    }

    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Mat<M, N> operator*(const Mat<M, N>& lhs, float scalar) noexcept {
        Mat<M, N> result;
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                result[i][j] = lhs[i][j] * scalar;
            }
        }
        return result;
    }

    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Mat<M, N> operator*(float scalar, const Mat<M, N>& rhs) noexcept {
        Mat<M, N> result;
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                result[i][j] = scalar * rhs[i][j];
            }
        }
        return result;
    }

    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Vec<float, N> operator*(const Mat<M, N>& lhs, const Vec<float, N>& rhs) noexcept {
        Vec<float, N> result;
        for (size_t i = 0; i < M; i++) {
            result[i] = dot(lhs[i], rhs);
        }
        return result;
    }

    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Vec<float, M> operator*(const Vec<float, M>& lhs, const Mat<M, N>& rhs) noexcept {
        Vec<float, M> result;
        for (size_t i = 0; i < M; i++) {
            result += lhs[i] * rhs[i];
        }
        return result;
    }

    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Mat<M, N>& operator+=(Mat<M, N>& lhs, const Mat<M, N>& rhs) noexcept {
        Mat<M, N> result;
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                lhs[i][j] += rhs[i][j];
            }
        }
        return lhs;
    }

    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Mat<M, N>& operator-=(Mat<M, N>& lhs, const Mat<M, N>& rhs) noexcept {
        Mat<M, N> result;
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                lhs[i][j] -= rhs[i][j];
            }
        }
        return lhs;
    }

    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    bool operator==(const Mat<M, N>& lhs, const Mat<M, N>& rhs) noexcept {
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                if (lhs[i][j] != rhs[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    bool operator!=(const Mat<M, N>& lhs, const Mat<M, N>& rhs) noexcept {
        return !(lhs == rhs);
    }

    /**
     * @param mat The matrix.
     * @param indent Indentation string before each row.
     * @return The string representation of the matrix.
     */
    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    [[nodiscard]] std::string toString(const Mat<M, N>& mat, const std::string& indent = "") {
        // Stringify each element
        std::array<std::string, M * N> elements;
        std::array<size_t, N> width{};
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                elements[i * N + j] = std::format("{}", mat[i][j]);
                if (elements[i * N + j].size() > width[j]) {
                    width[j] = elements[i * N + j].size();
                }
            }
        }

        // Format the string
        std::string result;
        for (size_t i = 0; i < M; i++) {
            if (i) result += "\n";
            result += indent + "[";
            for (size_t j = 0; j < N; j++) {
                if (j) result += ", ";
                result += std::format("{:>{}}", elements[i * N + j], width[j]);
            }
            result += "]";
        }
        return result;
    }

    using Mat2f = Mat<2, 2>;
    using Mat3f = Mat<3, 3>;
    using Mat4f = Mat<4, 4>;
}

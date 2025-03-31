#pragma once

#include <array>
#include <concepts>
#include <format>
#include <vector>
#include <corn/geometry/vec.h>

namespace corn {
    // Helper functions
    void invHelper(size_t N, const std::vector<float>& mat, std::vector<float>& inv) noexcept;
    void detHelper(size_t N, const std::vector<float>& mat, float& det) noexcept;
    void svdHelper(size_t M, size_t N, const std::vector<float>& mat, std::vector<float>& U, std::vector<float>& S, std::vector<float>& V) noexcept;

    /**
     * @class Mat
     * @brief Matrix class.
     * @tparam M Number of rows. Must be a positive integer.
     * @tparam N Number of columns. Must be a positive integer.
     */
    template <size_t M, size_t N>
    class Mat {
    public:
        /// @brief Constructor.
        Mat() noexcept = default;

        /// @brief Constructor.
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

        /// @brief Constructor.
        template <typename... Args>
        requires(sizeof...(Args) == M && std::conjunction_v<std::is_same<Args, Vec<float, N>>...>)
        explicit Mat(Args... args) noexcept : data_{ std::forward<Args>(args)... } {}

        /// @return Zero matrix.
        [[nodiscard]] static const Mat<M, N>& O() noexcept {
            static const Mat zero;
            return zero;
        }

        /// @return Identity matrix.
        [[nodiscard]] static const Mat<M, N>& I() noexcept
        requires(M == N) {
            static const Mat<M, N> identity = [] {
                Mat<M, N> mat;
                for (size_t i = 0; i < M; i++) {
                    mat.data_[i][i] = 1.0f;
                }
                return mat;
            }();
            return identity;
        }

        /**
         * @param diag Diagonal elements.
         * @return Diagonal matrix with the given diagonal elements.
         */
        [[nodiscard]] static Mat<M, N> diag(const Vec<float, M>& diag) noexcept
        requires(M == N) {
            Mat<M, N> mat;
            for (size_t i = 0; i < M; i++) {
                mat.data_[i][i] = diag[i];
            }
            return mat;
        }

        /// @return The number of rows.
        [[nodiscard]] static constexpr size_t rows() noexcept {
            return M;
        }

        /// @return The number of columns.
        [[nodiscard]] static constexpr size_t cols() noexcept {
            return N;
        }

        /**
         * @param index Index of the row.
         * @return Reference to the row at the given index.
         */
        [[nodiscard]] Vec<float, N>& row(size_t index) noexcept {
            return this->data_.at(index);
        }

        /**
         * @param index Index of the row.
         * @return Reference to the row at the given index.
         */
        [[nodiscard]] const Vec<float, N>& row(size_t index) const noexcept {
            return this->data_.at(index);
        }

        /**
         * @param index Index of the column.
         * @return Copy of the column at the given index.
         */
        [[nodiscard]] Vec<float, N> col(size_t index) const noexcept {
            Vec<float, M> column;
            for (size_t i = 0; i < M; ++i) {
                column[i] = this->data_[i].at(index);
            }
            return column;
        }

        /**
         * @param index Index of the row.
         * @return Reference to the row at the given index.
         */
        [[nodiscard]] Vec<float, N>& operator[](size_t index) noexcept {
            return this->data_[index];
        }

        /**
         * @param index Index of the row.
         * @return Reference to the row at the given index.
         */
        [[nodiscard]] const Vec<float, N>& operator[](size_t index) const noexcept {
            return this->data_[index];
        }

        /**
         * @param r Row index.
         * @param c Column index.
         * @return Reference to the element at the given row and column.
         */
        [[nodiscard]] float& operator()(size_t r, size_t c) noexcept {
            return this->data_[r][c];
        }

        /**
         * @param r Row index.
         * @param c Column index.
         * @return Reference to the element at the given row and column.
         */
        [[nodiscard]] const float& operator()(size_t r, size_t c) const noexcept {
            return this->data_[r][c];
        }

        /**
         * @param r Row index.
         * @return Reference to the row at the given index.
         */
        [[nodiscard]] Vec<float, N>& at(size_t r) noexcept {
            return this->data_.at(r);
        }

        /**
         * @param r Row index.
         * @return Reference to the row at the given index.
         */
        [[nodiscard]] const Vec<float, N>& at(size_t r) const noexcept {
            return this->data_.at(r);
        }

        /**
         * @param r Row index.
         * @param c Column index.
         * @return Reference to the element at the given row and column.
         */
        [[nodiscard]] float& at(size_t r, size_t c) noexcept {
            return this->data_.at(r).at(c);
        }

        /**
         * @param r Row index.
         * @param c Column index.
         * @return Reference to the element at the given row and column.
         */
        [[nodiscard]] const float& at(size_t r, size_t c) const noexcept {
            return this->data_.at(r).at(c);
        }

        /// @return The matrix as a vector of floats.
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

        /// @return The transpose of the matrix.
        [[nodiscard]] Mat<N, M> T() const noexcept {
            Mat<N, M> transposed;
            for (size_t i = 0; i < M; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    transposed[j][i] = this->data_[i][j];
                }
            }
            return transposed;
        }

        /// @return The inverse of the matrix.
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

        /**
         * @brief Convert to a matrix of another dimension.
         * @tparam K Number of rows of the new matrix.
         * @tparam L Number of columns of the new matrix.
         * @return Converted matrix.
         */
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

        /**
         * @tparam K Number of rows to prepend.
         * @param args Rows to prepend.
         * @return New matrix with prepended rows.
         */
        template <size_t K, typename... Args>
        requires(K > 0 && sizeof...(Args) == K && std::conjunction_v<std::is_same<Args, Vec<float, N>>...>)
        [[nodiscard]] Mat<M + K, N> prependRows(Args... args) const noexcept {
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

        /**
         * @tparam K Number of columns to prepend.
         * @param args Columns to prepend.
         * @return New matrix with prepended columns.
         */
        template <size_t K, typename... Args>
        requires(K > 0 && sizeof...(Args) == K && std::conjunction_v<std::is_same<Args, Vec<float, M>>...>)
        [[nodiscard]] Mat<M, N + K> prependCols(Args... args) const noexcept {
            Mat<M, N + K> result;
            std::array<Vec<float, N>, K> prefix = { std::forward<Args>(args)... };
            for (size_t i = 0; i < M; i++) {
                for (size_t j = 0; j < K; j++) {
                    result[i][j] = prefix[j][i];
                }
                for (size_t j = 0; j < N; j++) {
                    result[i][K + j] = this->data_[i][j];
                }
            }
            return result;
        }

        /**
         * @tparam K Number of rows to append.
         * @param args Rows to append.
         * @return New matrix with appended rows.
         */
        template <size_t K, typename... Args>
        requires(K > 0 && sizeof...(Args) == K && std::conjunction_v<std::is_same<Args, Vec<float, N>>...>)
        [[nodiscard]] Mat<M + K, N> appendRows(Args... args) const noexcept {
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

        /**
         * @tparam K Number of columns to append.
         * @param args Columns to append.
         * @return New matrix with appended columns.
         */
        template <size_t K, typename... Args>
        requires(K > 0 && sizeof...(Args) == K && std::conjunction_v<std::is_same<Args, Vec<float, M>>...>)
        [[nodiscard]] Mat<M, N + K> appendCols(Args... args) const noexcept {
            Mat<M, N + K> result;
            std::array<Vec<float, N>, K> prefix = { std::forward<Args>(args)... };
            for (size_t i = 0; i < M; i++) {
                for (size_t j = 0; j < N; j++) {
                    result[i][j] = this->data_[i][j];
                }
                for (size_t j = 0; j < K; j++) {
                    result[i][K + j] = prefix[j][i];
                }
            }
            return result;
        }

        /// @return The determinant of the matrix.
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
                float det = 0.0f;
                detHelper(N, flattened, det);
                return det;
            }
        }

        /**
         * @brief Singular Value Decomposition (SVD).
         * @param U Left singular vectors.
         * @param S Singular values.
         * @param V Right singular vectors.
         */
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

    // Operators
    /// @return A copy of the matrix itself.
    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Mat<M, N> operator+(const Mat<M, N>& rhs) noexcept {
        return rhs;
    }

    /// @return The additive inverse of the matrix.
    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Mat<M, N> operator-(const Mat<M, N>& rhs) noexcept {
        Mat<M, N> result;
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                result[i][j] = -rhs[i][j];
            }
        }
        return result;
    }

    /// @return Result of adding lhs and rhs.
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

    /// @return Result of subtracting rhs from lhs.
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

    /// @return Matrix multiplication of lhs and rhs.
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

    /// @return Result of multiplying a matrix and a scalar.
    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Mat<M, N> operator*(const Mat<M, N>& mat, float scalar) noexcept {
        Mat<M, N> result;
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                result[i][j] = mat[i][j] * scalar;
            }
        }
        return result;
    }

    /// @return Result of multiplying a scalar and a matrix.
    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Mat<M, N> operator*(float scalar, const Mat<M, N>& mat) noexcept {
        Mat<M, N> result;
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                result[i][j] = scalar * mat[i][j];
            }
        }
        return result;
    }

    /// @return Result of multiplying a matrix and a vector.
    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Vec<float, N> operator*(const Mat<M, N>& mat, const Vec<float, N>& vec) noexcept {
        Vec<float, N> result;
        for (size_t i = 0; i < M; i++) {
            result[i] = dot(mat[i], vec);
        }
        return result;
    }

    /// @return Result of multiplying a vector and a matrix.
    template <size_t M, size_t N>
    requires(M > 0 && N > 0)
    Vec<float, M> operator*(const Vec<float, M>& vec, const Mat<M, N>& mat) noexcept {
        Vec<float, M> result;
        for (size_t i = 0; i < M; i++) {
            result += vec[i] * mat[i];
        }
        return result;
    }

    /**
     * @brief Add rhs to lhs matrix in-place.
     * @param lhs First matrix.
     * @param rhs Second matrix.
     * @return Reference to lhs.
     */
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

    /**
     * @brief Subtract rhs from lhs matrix in-place.
     * @param lhs First matrix.
     * @param rhs Second matrix.
     * @return Reference to lhs.
     */
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

    /**
     * @brief Compare two matrices for equality.
     * @param lhs First matrix.
     * @param rhs Second matrix.
     * @return Whether the matrices are equal.
     */
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

    /**
     * @brief Compare two matrices for inequality.
     * @param lhs First matrix.
     * @param rhs Second matrix.
     * @return Whether the matrices are not equal.
     */
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

#include <Eigen/Dense>
#include <corn/geometry/mat.h>

namespace corn {
    void invHelper(size_t N, const std::vector<float>& mat, std::vector<float>& inv) noexcept {
        Eigen::MatrixXf eigenMat(N, N);
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < N; j++) {
                eigenMat((Eigen::Index)i, (Eigen::Index)j) = mat[i * N + j];
            }
        }

        inv.clear();
        inv.reserve(N * N);
        Eigen::MatrixXf invEigenMat = eigenMat.inverse();
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < N; j++) {
                inv.push_back(invEigenMat((Eigen::Index)i, (Eigen::Index)j));
            }
        }
    }

    void detHelper(size_t N, const std::vector<float>& mat, float& det) noexcept {
        Eigen::MatrixXf eigenMat(N, N);
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                eigenMat((Eigen::Index)i, (Eigen::Index)j) = mat[i * N + j];
            }
        }
        det = eigenMat.determinant();
    }

    void svdHelper(size_t M, size_t N, const std::vector<float>& input, std::vector<float>& U, std::vector<float>& S, std::vector<float>& V) noexcept {
        Eigen::MatrixXf eigenMat(M, N);
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                eigenMat((Eigen::Index)i, (Eigen::Index)j) = input[i * N + j];
            }
        }

        Eigen::JacobiSVD<Eigen::MatrixXf> svd(eigenMat, Eigen::ComputeThinU | Eigen::ComputeThinV);
        U.resize(M * M);
        S.resize(std::min(M, N));
        V.resize(N * N);

        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < M; ++j) {
                U[i * M + j] = svd.matrixU()((Eigen::Index)i, (Eigen::Index)j);
            }
        }

        for (size_t i = 0; i < std::min(M, N); ++i) {
            S[i] = svd.singularValues()((Eigen::Index)i);
        }

        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                V[i * N + j] = svd.matrixV()((Eigen::Index)i, (Eigen::Index)j);
            }
        }
    }
}

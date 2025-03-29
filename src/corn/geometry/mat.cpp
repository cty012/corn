#include <Eigen/Dense>
#include <corn/geometry/mat.h>

namespace corn {
    std::vector<float> invHelper(size_t N, const std::vector<float>& input) noexcept {
        Eigen::MatrixXf eigenMat(N, N);
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < N; j++) {
                eigenMat((Eigen::Index)i, (Eigen::Index)j) = input[i * N + j];
            }
        }

        std::vector<float> output;
        output.reserve(N * N);
        Eigen::MatrixXf invEigenMat = eigenMat.inverse();
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < N; j++) {
                output.push_back(invEigenMat((Eigen::Index)i, (Eigen::Index)j));
            }
        }

        return output;
    }

    float detHelper(size_t N, const std::vector<float>& input) noexcept {
        Eigen::MatrixXf eigenMat(N, N);
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                eigenMat((Eigen::Index)i, (Eigen::Index)j) = input[i * N + j];
            }
        }
        return eigenMat.determinant();
    }
}

#include <gtest/gtest.h>
#include <corn/geometry/mat.h>

namespace corn::test::mat {
    TEST(Mat, ctor) {
        Mat<2, 3> mat1;
        EXPECT_FLOAT_EQ(mat1[0][0], 0.0f);
        EXPECT_FLOAT_EQ(mat1[0][1], 0.0f);
        EXPECT_FLOAT_EQ(mat1[0][2], 0.0f);
        EXPECT_FLOAT_EQ(mat1[1][0], 0.0f);
        EXPECT_FLOAT_EQ(mat1[1][1], 0.0f);
        EXPECT_FLOAT_EQ(mat1[1][2], 0.0f);

        Mat<2, 3> mat2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
        EXPECT_FLOAT_EQ(mat2[0][0], 1.0f);
        EXPECT_FLOAT_EQ(mat2[0][1], 2.0f);
        EXPECT_FLOAT_EQ(mat2[0][2], 3.0f);
        EXPECT_FLOAT_EQ(mat2[1][0], 4.0f);
        EXPECT_FLOAT_EQ(mat2[1][1], 5.0f);
        EXPECT_FLOAT_EQ(mat2[1][2], 6.0f);

        Mat<2, 3> mat3(Vec<float, 3>(1.0f, 2.0f, 3.0f), Vec<float, 3>(4.0f, 5.0f, 6.0f));
        EXPECT_FLOAT_EQ(mat3[0][0], 1.0f);
        EXPECT_FLOAT_EQ(mat3[0][1], 2.0f);
        EXPECT_FLOAT_EQ(mat3[0][2], 3.0f);
        EXPECT_FLOAT_EQ(mat3[1][0], 4.0f);
        EXPECT_FLOAT_EQ(mat3[1][1], 5.0f);
        EXPECT_FLOAT_EQ(mat3[1][2], 6.0f);

        Mat<2, 3> mat4(std::vector<float>{ 1.0f, 2.0f });
        EXPECT_FLOAT_EQ(mat4[0][0], 1.0f);
        EXPECT_FLOAT_EQ(mat4[0][1], 2.0f);
        EXPECT_FLOAT_EQ(mat4[0][2], 0.0f);
        EXPECT_FLOAT_EQ(mat4[1][0], 0.0f);
        EXPECT_FLOAT_EQ(mat4[1][1], 0.0f);
        EXPECT_FLOAT_EQ(mat4[1][2], 0.0f);

        Mat<2, 3> mat5(std::vector<float>{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f });
        EXPECT_FLOAT_EQ(mat5[0][0], 1.0f);
        EXPECT_FLOAT_EQ(mat5[0][1], 2.0f);
        EXPECT_FLOAT_EQ(mat5[0][2], 3.0f);
        EXPECT_FLOAT_EQ(mat5[1][0], 4.0f);
        EXPECT_FLOAT_EQ(mat5[1][1], 5.0f);
        EXPECT_FLOAT_EQ(mat5[1][2], 6.0f);

        Mat<2, 3> mat6(std::vector<float>{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f });
        EXPECT_FLOAT_EQ(mat6[0][0], 1.0f);
        EXPECT_FLOAT_EQ(mat6[0][1], 2.0f);
        EXPECT_FLOAT_EQ(mat6[0][2], 3.0f);
        EXPECT_FLOAT_EQ(mat6[1][0], 4.0f);
        EXPECT_FLOAT_EQ(mat6[1][1], 5.0f);
        EXPECT_FLOAT_EQ(mat6[1][2], 6.0f);
    }

    TEST(Mat, constants) {
        Mat<2, 3> mat1 = Mat<2, 3>::O();
        EXPECT_FLOAT_EQ(mat1[0][0], 0.0f);
        EXPECT_FLOAT_EQ(mat1[0][1], 0.0f);
        EXPECT_FLOAT_EQ(mat1[0][2], 0.0f);
        EXPECT_FLOAT_EQ(mat1[1][0], 0.0f);
        EXPECT_FLOAT_EQ(mat1[1][1], 0.0f);
        EXPECT_FLOAT_EQ(mat1[1][2], 0.0f);

        Mat<3, 3> mat2 = Mat<3, 3>::I();
        EXPECT_FLOAT_EQ(mat2[0][0], 1.0f);
        EXPECT_FLOAT_EQ(mat2[0][1], 0.0f);
        EXPECT_FLOAT_EQ(mat2[0][2], 0.0f);
        EXPECT_FLOAT_EQ(mat2[1][0], 0.0f);
        EXPECT_FLOAT_EQ(mat2[1][1], 1.0f);
        EXPECT_FLOAT_EQ(mat2[1][2], 0.0f);
        EXPECT_FLOAT_EQ(mat2[2][0], 0.0f);
        EXPECT_FLOAT_EQ(mat2[2][1], 0.0f);
        EXPECT_FLOAT_EQ(mat2[2][2], 1.0f);
    }

    TEST(Mat, transpose) {
        Mat<2, 3> mat1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
        Mat<3, 2> result1 = mat1.T();
        EXPECT_FLOAT_EQ(result1[0][0], 1.0f);
        EXPECT_FLOAT_EQ(result1[0][1], 4.0f);
        EXPECT_FLOAT_EQ(result1[1][0], 2.0f);
        EXPECT_FLOAT_EQ(result1[1][1], 5.0f);
        EXPECT_FLOAT_EQ(result1[2][0], 3.0f);
        EXPECT_FLOAT_EQ(result1[2][1], 6.0f);
    }

    TEST(Mat, determinant) {
        Mat<2, 2> mat1(
                1.0f, 2.0f,
                3.0f, 4.0f);
        EXPECT_FLOAT_EQ(mat1.det(), -2.0f);

        Mat<3, 3> mat2(
                1.0f, 2.0f, 3.0f,
                4.0f, 5.0f, 6.0f,
                7.0f, 8.0f, 9.0f);
        EXPECT_FLOAT_EQ(mat2.det(), 0.0f);

        Mat<4, 4> mat3(
                1.0f, 2.0f, 3.0f, 4.0f,
                5.0f, 6.0f, 7.0f, 8.0f,
                9.0f, 10.0f, 11.0f, 12.0f,
                13.0f, 14.0f, 15.0f, 16.0f);
        EXPECT_FLOAT_EQ(mat3.det(), 0.0f);
    }

    TEST(Mat, addition) {
        Mat<2, 3> mat1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
        Mat<2, 3> mat2(7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
        Mat<2, 3> result = mat1 + mat2;
        EXPECT_FLOAT_EQ(result[0][0], 8.0f);
        EXPECT_FLOAT_EQ(result[0][1], 10.0f);
        EXPECT_FLOAT_EQ(result[0][2], 12.0f);
        EXPECT_FLOAT_EQ(result[1][0], 14.0f);
        EXPECT_FLOAT_EQ(result[1][1], 16.0f);
        EXPECT_FLOAT_EQ(result[1][2], 18.0f);
    }

    TEST(Mat, multiplication) {
        Mat<2, 3> mat1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
        Mat<3, 2> mat2(7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
        Mat<2, 2> result = mat1 * mat2;
        EXPECT_FLOAT_EQ(result[0][0], 58.0f);
        EXPECT_FLOAT_EQ(result[0][1], 64.0f);
        EXPECT_FLOAT_EQ(result[1][0], 139.0f);
        EXPECT_FLOAT_EQ(result[1][1], 154.0f);
    }
}

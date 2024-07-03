#include <gtest/gtest.h>
#include <corn/geometry/vec4.h>

namespace corn::test {
    TEST(Vec4, default_ctor) {
        Vec4 v;
        EXPECT_FLOAT_EQ(v.x, 0.0f);
        EXPECT_FLOAT_EQ(v.y, 0.0f);
        EXPECT_FLOAT_EQ(v.z, 0.0f);
        EXPECT_FLOAT_EQ(v.w, 0.0f);
    }

    TEST(Vec4, parameterized_ctor) {
        Vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
        EXPECT_FLOAT_EQ(v.x, 1.0f);
        EXPECT_FLOAT_EQ(v.y, 2.0f);
        EXPECT_FLOAT_EQ(v.z, 3.0f);
        EXPECT_FLOAT_EQ(v.w, 4.0f);
    }

    TEST(Vec4, static_zero) {
        const Vec4& zero = Vec4::ZERO();
        EXPECT_FLOAT_EQ(zero.x, 0.0f);
        EXPECT_FLOAT_EQ(zero.y, 0.0f);
        EXPECT_FLOAT_EQ(zero.z, 0.0f);
        EXPECT_FLOAT_EQ(zero.w, 0.0f);
    }

    TEST(Vec4, normalize) {
        Vec4 v(3.0f, 4.0f, 12.0f, 84.0f);
        float norm = v.norm();
        Vec4 normalized = v.normalize();
        EXPECT_FLOAT_EQ(norm, 85.0f);
        EXPECT_NEAR(normalized.norm(), 1.0f, 1e-6);
        EXPECT_FLOAT_EQ(normalized.x * norm, v.x);
        EXPECT_FLOAT_EQ(normalized.y * norm, v.y);
        EXPECT_FLOAT_EQ(normalized.z * norm, v.z);
        EXPECT_FLOAT_EQ(normalized.w * norm, v.w);
    }

    TEST(Vec4, addition) {
        Vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
        Vec4 result = v1 + v2;
        EXPECT_FLOAT_EQ(result.x, 6.0f);
        EXPECT_FLOAT_EQ(result.y, 8.0f);
        EXPECT_FLOAT_EQ(result.z, 10.0f);
        EXPECT_FLOAT_EQ(result.w, 12.0f);

        // In place
        v1 += v2;
        EXPECT_FLOAT_EQ(v1.x, 6.0f);
        EXPECT_FLOAT_EQ(v1.y, 8.0f);
        EXPECT_FLOAT_EQ(v1.z, 10.0f);
        EXPECT_FLOAT_EQ(v1.w, 12.0f);
    }

    TEST(Vec4, subtraction) {
        Vec4 v1(5.0f, 7.0f, 9.0f, 0.0f);
        Vec4 v2(4.0f, 3.0f, 2.0f, 1.0f);
        Vec4 result = v1 - v2;
        EXPECT_FLOAT_EQ(result.x, 1.0f);
        EXPECT_FLOAT_EQ(result.y, 4.0f);
        EXPECT_FLOAT_EQ(result.z, 7.0f);
        EXPECT_FLOAT_EQ(result.w, -1.0f);

        // In place
        v1 -= v2;
        EXPECT_FLOAT_EQ(v1.x, 1.0f);
        EXPECT_FLOAT_EQ(v1.y, 4.0f);
        EXPECT_FLOAT_EQ(v1.z, 7.0f);
        EXPECT_FLOAT_EQ(v1.w, -1.0f);
    }

    TEST(Vec4, multiplication_by_scalar) {
        Vec4 v(2.0f, 3.0f, 5.0f, 7.0f);
        Vec4 result1 = v * 2.0f;
        EXPECT_FLOAT_EQ(result1.x, 4.0f);
        EXPECT_FLOAT_EQ(result1.y, 6.0f);
        EXPECT_FLOAT_EQ(result1.z, 10.0f);
        EXPECT_FLOAT_EQ(result1.w, 14.0f);

        // In place
        v *= 2.0f;
        EXPECT_FLOAT_EQ(v.x, 4.0f);
        EXPECT_FLOAT_EQ(v.y, 6.0f);
        EXPECT_FLOAT_EQ(v.z, 10.0f);
        EXPECT_FLOAT_EQ(v.w, 14.0f);
    }

    TEST(Vec4, multiplication_by_vector) {
        Vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
        Vec4 result = v1 * v2;
        EXPECT_FLOAT_EQ(result.x, 5.0f);
        EXPECT_FLOAT_EQ(result.y, 12.0f);
        EXPECT_FLOAT_EQ(result.z, 21.0f);
        EXPECT_FLOAT_EQ(result.w, 32.0f);

        // In place
        v1 *= v2;
        EXPECT_FLOAT_EQ(v1.x, 5.0f);
        EXPECT_FLOAT_EQ(v1.y, 12.0f);
        EXPECT_FLOAT_EQ(v1.z, 21.0f);
        EXPECT_FLOAT_EQ(v1.w, 32.0f);
    }
}

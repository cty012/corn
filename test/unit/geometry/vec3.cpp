#include <gtest/gtest.h>
#include <corn/geometry/vec3.h>

namespace corn::test::vec3 {
    TEST(Vec3, default_ctor) {
        Vec3 v;
        EXPECT_FLOAT_EQ(v.x, 0.0f);
        EXPECT_FLOAT_EQ(v.y, 0.0f);
        EXPECT_FLOAT_EQ(v.z, 0.0f);
    }

    TEST(Vec3, parameterized_ctor) {
        Vec3 v(1.0f, 2.0f, 3.0f);
        EXPECT_FLOAT_EQ(v.x, 1.0f);
        EXPECT_FLOAT_EQ(v.y, 2.0f);
        EXPECT_FLOAT_EQ(v.z, 3.0f);
    }

    TEST(Vec3, static_zero) {
        const Vec3& zero = Vec3::ZERO();
        EXPECT_FLOAT_EQ(zero.x, 0.0f);
        EXPECT_FLOAT_EQ(zero.y, 0.0f);
        EXPECT_FLOAT_EQ(zero.z, 0.0f);
    }

    TEST(Vec3, normalize) {
        Vec3 v(3.0f, 4.0f, 12.0f);
        float norm = v.norm();
        Vec3 normalized = v.normalize();
        EXPECT_FLOAT_EQ(norm, 13.0f);
        EXPECT_NEAR(normalized.norm(), 1.0f, 1e-6);
        EXPECT_FLOAT_EQ(normalized.x * norm, v.x);
        EXPECT_FLOAT_EQ(normalized.y * norm, v.y);
        EXPECT_FLOAT_EQ(normalized.z * norm, v.z);
    }

    TEST(Vec3, addition) {
        Vec3 v1(1.0f, 2.0f, 3.0f);
        Vec3 v2(4.0f, 5.0f, 6.0f);
        Vec3 result = v1 + v2;
        EXPECT_FLOAT_EQ(result.x, 5.0f);
        EXPECT_FLOAT_EQ(result.y, 7.0f);
        EXPECT_FLOAT_EQ(result.z, 9.0f);

        // In place
        v1 += v2;
        EXPECT_FLOAT_EQ(v1.x, 5.0f);
        EXPECT_FLOAT_EQ(v1.y, 7.0f);
        EXPECT_FLOAT_EQ(v1.z, 9.0f);
    }

    TEST(Vec3, subtraction) {
        Vec3 v1(5.0f, 7.0f, 9.0f);
        Vec3 v2(4.0f, 3.0f, 2.0f);
        Vec3 result = v1 - v2;
        EXPECT_FLOAT_EQ(result.x, 1.0f);
        EXPECT_FLOAT_EQ(result.y, 4.0f);
        EXPECT_FLOAT_EQ(result.z, 7.0f);

        // In place
        v1 -= v2;
        EXPECT_FLOAT_EQ(v1.x, 1.0f);
        EXPECT_FLOAT_EQ(v1.y, 4.0f);
        EXPECT_FLOAT_EQ(v1.z, 7.0f);
    }

    TEST(Vec3, multiplication_by_scalar) {
        Vec3 v(2.0f, 3.0f, 5.0f);
        Vec3 result1 = v * 2.0f;
        EXPECT_FLOAT_EQ(result1.x, 4.0f);
        EXPECT_FLOAT_EQ(result1.y, 6.0f);
        EXPECT_FLOAT_EQ(result1.z, 10.0f);

        // In place
        v *= 2.0f;
        EXPECT_FLOAT_EQ(v.x, 4.0f);
        EXPECT_FLOAT_EQ(v.y, 6.0f);
        EXPECT_FLOAT_EQ(v.z, 10.0f);
    }

    TEST(Vec3, multiplication_by_vector) {
        Vec3 v1(1.0f, 2.0f, 3.0f);
        Vec3 v2(4.0f, 5.0f, 6.0f);
        Vec3 result = v1 * v2;
        EXPECT_FLOAT_EQ(result.x, 4.0f);
        EXPECT_FLOAT_EQ(result.y, 10.0f);
        EXPECT_FLOAT_EQ(result.z, 18.0f);

        // In place
        v1 *= v2;
        EXPECT_FLOAT_EQ(v1.x, 4.0f);
        EXPECT_FLOAT_EQ(v1.y, 10.0f);
        EXPECT_FLOAT_EQ(v1.z, 18.0f);
    }
}

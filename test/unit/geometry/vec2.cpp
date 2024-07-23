#include <gtest/gtest.h>
#include <corn/geometry/vec2.h>

namespace corn::test::vec2 {
    TEST(Vec2, default_ctor) {
        Vec2 v;
        EXPECT_FLOAT_EQ(v.x, 0.0f);
        EXPECT_FLOAT_EQ(v.y, 0.0f);
    }

    TEST(Vec2, parameterized_ctor) {
        Vec2 v(1.0f, 2.0f);
        EXPECT_FLOAT_EQ(v.x, 1.0f);
        EXPECT_FLOAT_EQ(v.y, 2.0f);
    }

    TEST(Vec2, static_zero) {
        const Vec2& zero = Vec2::ZERO();
        EXPECT_FLOAT_EQ(zero.x, 0.0f);
        EXPECT_FLOAT_EQ(zero.y, 0.0f);
    }

    TEST(Vec2, norm) {
        Vec2 v(3.0f, 4.0f);
    }

    TEST(Vec2, normalize) {
        Vec2 v(3.0f, 4.0f);
        float norm = v.norm();
        Vec2 normalized = v.normalize();
        EXPECT_FLOAT_EQ(norm, 5.0f);
        EXPECT_NEAR(normalized.norm(), 1.0f, 1e-6);
        EXPECT_FLOAT_EQ(normalized.x * norm, v.x);
        EXPECT_FLOAT_EQ(normalized.y * norm, v.y);
    }

    TEST(Vec2, addition) {
        Vec2 v1(1.0f, 2.0f);
        Vec2 v2(3.0f, 4.0f);
        Vec2 result = v1 + v2;
        EXPECT_FLOAT_EQ(result.x, 4.0f);
        EXPECT_FLOAT_EQ(result.y, 6.0f);

        // In place
        v1 += v2;
        EXPECT_FLOAT_EQ(v1.x, 4.0f);
        EXPECT_FLOAT_EQ(v1.y, 6.0f);
    }

    TEST(Vec2, subtraction) {
        Vec2 v1(5.0f, 6.0f);
        Vec2 v2(3.0f, 4.0f);
        Vec2 result = v1 - v2;
        EXPECT_FLOAT_EQ(result.x, 2.0f);
        EXPECT_FLOAT_EQ(result.y, 2.0f);

        // In place
        v1 -= v2;
        EXPECT_FLOAT_EQ(v1.x, 2.0f);
        EXPECT_FLOAT_EQ(v1.y, 2.0f);
    }

    TEST(Vec2, multiplication_by_scalar) {
        Vec2 v(2.0f, 3.0f);
        Vec2 result = v * 2.0f;
        EXPECT_FLOAT_EQ(result.x, 4.0f);
        EXPECT_FLOAT_EQ(result.y, 6.0f);

        // In place
        v *= 2.0f;
        EXPECT_FLOAT_EQ(v.x, 4.0f);
        EXPECT_FLOAT_EQ(v.y, 6.0f);
    }

    TEST(Vec2, multiplication_by_vector) {
        Vec2 v1(1.0f, 2.0f);
        Vec2 v2(3.0f, 4.0f);
        Vec2 result = v1 * v2;
        EXPECT_FLOAT_EQ(result.x, 3.0f);
        EXPECT_FLOAT_EQ(result.y, 8.0f);

        // In place
        v1 *= v2;
        EXPECT_FLOAT_EQ(v1.x, 3.0f);
        EXPECT_FLOAT_EQ(v1.y, 8.0f);
    }
}

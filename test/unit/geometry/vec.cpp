#include <gtest/gtest.h>
#include <corn/geometry/vec.h>

namespace corn::test::vec {
    TEST(Vec, default_ctor) {
        Vec2f v1;
        EXPECT_FLOAT_EQ(v1.x, 0.0f);
        EXPECT_FLOAT_EQ(v1.y, 0.0f);

        Vec2f v2(1.0f, 2.0f);
        EXPECT_FLOAT_EQ(v2.x, 1.0f);
        EXPECT_FLOAT_EQ(v2.y, 2.0f);

        Vec3f v3(std::vector<float>{ 1.0f, 2.0f });
        EXPECT_FLOAT_EQ(v3.x, 1.0f);
        EXPECT_FLOAT_EQ(v3.y, 2.0f);
        EXPECT_FLOAT_EQ(v3.z, 0.0f);

        Vec3f v4(std::vector<float>{ 1.0f, 2.0f, 3.0f });
        EXPECT_FLOAT_EQ(v4.x, 1.0f);
        EXPECT_FLOAT_EQ(v4.y, 2.0f);
        EXPECT_FLOAT_EQ(v4.z, 3.0f);

        Vec4f v5(std::vector<float>{ 1.0f, 2.0f, 3.0f, 4.0f });
        EXPECT_FLOAT_EQ(v5.x, 1.0f);
        EXPECT_FLOAT_EQ(v5.y, 2.0f);
        EXPECT_FLOAT_EQ(v5.z, 3.0f);

        Vec2f v6(1.0f, 2.0f);
        Vec2f v7(v6);
        EXPECT_FLOAT_EQ(v7.x, 1.0f);
        EXPECT_FLOAT_EQ(v7.y, 2.0f);
    }

    TEST(Vec, constants) {
        const Vec2f& zero = Vec2f::O();
        EXPECT_FLOAT_EQ(zero.x, 0.0f);
        EXPECT_FLOAT_EQ(zero.y, 0.0f);

        const Vec2f& ex2 = Vec2f::E_X();
        EXPECT_FLOAT_EQ(ex2.x, 1.0f);
        EXPECT_FLOAT_EQ(ex2.y, 0.0f);

        const Vec3f& ez3 = Vec3f::E_Z();
        EXPECT_FLOAT_EQ(ez3.x, 0.0f);
        EXPECT_FLOAT_EQ(ez3.y, 0.0f);
        EXPECT_FLOAT_EQ(ez3.z, 1.0f);

        const Vec3f& ex3 = Vec3f::E<0>();
        EXPECT_FLOAT_EQ(ex3.x, 1.0f);
        EXPECT_FLOAT_EQ(ex3.y, 0.0f);
        EXPECT_FLOAT_EQ(ex3.z, 0.0f);
    }

    TEST(Vec, norm) {
        Vec2f v(3.0f, 4.0f);
        EXPECT_FLOAT_EQ(v.norm(), 5.0f);
        EXPECT_FLOAT_EQ(v.norm2(), 25.0f);
    }

    TEST(Vec, normalize) {
        Vec2f v(3.0f, 4.0f);
        float norm = v.norm();
        Vec2f normalized = v.normalize();
        EXPECT_FLOAT_EQ(norm, 5.0f);
        EXPECT_NEAR(normalized.norm(), 1.0f, 1e-6);
        EXPECT_FLOAT_EQ(normalized.x * norm, v.x);
        EXPECT_FLOAT_EQ(normalized.y * norm, v.y);
    }

    TEST(Vec, addition) {
        Vec2f v1(1.0f, 2.0f);
        Vec2f v2(3.0f, 4.0f);
        Vec2f result = v1 + v2;
        EXPECT_FLOAT_EQ(result.x, 4.0f);
        EXPECT_FLOAT_EQ(result.y, 6.0f);

        // In place
        v1 += v2;
        EXPECT_FLOAT_EQ(v1.x, 4.0f);
        EXPECT_FLOAT_EQ(v1.y, 6.0f);
    }

    TEST(Vec, subtraction) {
        Vec2f v1(5.0f, 6.0f);
        Vec2f v2(3.0f, 4.0f);
        Vec2f result = v1 - v2;
        EXPECT_FLOAT_EQ(result.x, 2.0f);
        EXPECT_FLOAT_EQ(result.y, 2.0f);

        // In place
        v1 -= v2;
        EXPECT_FLOAT_EQ(v1.x, 2.0f);
        EXPECT_FLOAT_EQ(v1.y, 2.0f);
    }

    TEST(Vec, multiplication_by_scalar) {
        Vec2f v(2.0f, 3.0f);
        Vec2f result = v * 2.0f;
        EXPECT_FLOAT_EQ(result.x, 4.0f);
        EXPECT_FLOAT_EQ(result.y, 6.0f);

        // In place
        v *= 2.0f;
        EXPECT_FLOAT_EQ(v.x, 4.0f);
        EXPECT_FLOAT_EQ(v.y, 6.0f);
    }

    TEST(Vec, multiplication_by_vector) {
        Vec2f v1(1.0f, 2.0f);
        Vec2f v2(3.0f, 4.0f);
        Vec2f result = v1 * v2;
        EXPECT_FLOAT_EQ(result.x, 3.0f);
        EXPECT_FLOAT_EQ(result.y, 8.0f);

        // In place
        v1 *= v2;
        EXPECT_FLOAT_EQ(v1.x, 3.0f);
        EXPECT_FLOAT_EQ(v1.y, 8.0f);
    }
}

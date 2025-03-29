#include <gtest/gtest.h>
#include <corn/geometry/transform.h>
#include "corn/geometry/operations.h"

namespace corn::test::transform {
    TEST(Transform2D, ctor) {
        Transform2D transform1;
        Mat3f mat1(
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f);
        EXPECT_TRUE(transform1.getMat() == mat1);

        Transform2D transform2(Mat3f(
                1.0f, 2.0f, 3.0f,
                4.0f, 5.0f, 6.0f,
                7.0f, 8.0f, 9.0f));
        Mat3f mat2(
                1.0f, 2.0f, 3.0f,
                4.0f, 5.0f, 6.0f,
                7.0f, 8.0f, 9.0f);
        EXPECT_TRUE(transform2.getMat() == mat2);
    }

    TEST(Transform2D, constants) {
        const Transform2D& identity = Transform2D::I();
        Mat3f expectedIdentity(
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f);
        EXPECT_TRUE(identity.getMat() == expectedIdentity);
    }

    TEST(Transform2D, base_transforms) {
        Vec2f point(3.0f, 4.0f);
        Vec2f vector(3.0f, 4.0f);

        Transform2D translation = Transform2D::translate(Vec2f(1.0f, 2.0f));
        {
            Mat3f expectedTranslation(
                    1.0f, 0.0f, 1.0f,
                    0.0f, 1.0f, 2.0f,
                    0.0f, 0.0f, 1.0f);
            EXPECT_TRUE(translation.getMat() == expectedTranslation);

            Vec2f mappedPoint = translation.mapPoint(point);
            Vec2f expectedMappedPoint(4.0f, 6.0f);
            EXPECT_FLOAT_EQ(mappedPoint.x, expectedMappedPoint.x);
            EXPECT_FLOAT_EQ(mappedPoint.y, expectedMappedPoint.y);

            Vec2f mappedVector = translation.mapVector(vector);
            Vec2f expectedMappedVector(3.0f, 4.0f);
            EXPECT_FLOAT_EQ(mappedVector.x, expectedMappedVector.x);
            EXPECT_FLOAT_EQ(mappedVector.y, expectedMappedVector.y);
        }

        Transform2D rotation = Transform2D::rotate(Deg(30.0f));
        {
            Mat3f expectedRotation(
                    0.5f * std::sqrt(3.0f), -0.5f, 0.0f,
                    0.5f, 0.5f * std::sqrt(3.0f), 0.0f,
                    0.0f, 0.0f, 1.0f);
            EXPECT_TRUE(rotation.getMat() == expectedRotation);

            Vec2f mappedPoint = rotation.mapPoint(point);
            Vec2f expectedMappedPoint(
                    0.5f * std::sqrt(3.0f) * 3.0f - 0.5f * 4.0f,
                    0.5f * 3.0f + 0.5f * std::sqrt(3.0f) * 4.0f);
            EXPECT_FLOAT_EQ(mappedPoint.x, expectedMappedPoint.x);
            EXPECT_FLOAT_EQ(mappedPoint.y, expectedMappedPoint.y);

            Vec2f mappedVector = rotation.mapVector(vector);
            Vec2f expectedMappedVector(
                    0.5f * std::sqrt(3.0f) * 3.0f - 0.5f * 4.0f,
                    0.5f * 3.0f + 0.5f * std::sqrt(3.0f) * 4.0f);
            EXPECT_FLOAT_EQ(mappedVector.x, expectedMappedVector.x);
            EXPECT_FLOAT_EQ(mappedVector.y, expectedMappedVector.y);
        }

        Transform2D dilation = Transform2D::dilate(Vec2f(1.5f, 2.0f));
        {
            Mat3f expectedDilation(
                    1.5f, 0.0f, 0.0f,
                    0.0f, 2.0f, 0.0f,
                    0.0f, 0.0f, 1.0f);
            EXPECT_TRUE(dilation.getMat() == expectedDilation);

            Vec2f mappedPoint = dilation.mapPoint(point);
            Vec2f expectedMappedPoint(4.5f, 8.0f);
            EXPECT_FLOAT_EQ(mappedPoint.x, expectedMappedPoint.x);
            EXPECT_FLOAT_EQ(mappedPoint.y, expectedMappedPoint.y);

            Vec2f mappedVector = dilation.mapVector(vector);
            Vec2f expectedMappedVector(4.5f, 8.0f);
            EXPECT_FLOAT_EQ(mappedVector.x, expectedMappedVector.x);
            EXPECT_FLOAT_EQ(mappedVector.y, expectedMappedVector.y);
        }
    }

    TEST(Transform2D, mapPoint) {
        Vec2f point(3.0f, 4.0f);
        Vec2f vector(3.0f, 4.0f);

        Transform2D transform(Vec2f(1.0f, 2.0f), Deg(90.0f), Vec2f(3.0f, 4.0f));
        {
            Mat3f expectedTransform(
                    0.0f, -4.0f, 1.0f,
                    3.0f, 0.0f, 2.0f,
                    0.0f, 0.0f, 1.0f);
            EXPECT_NEAR(transform.getMat()[0][0], expectedTransform[0][0], 1e-6);
            EXPECT_NEAR(transform.getMat()[0][1], expectedTransform[0][1], 1e-6);
            EXPECT_NEAR(transform.getMat()[0][2], expectedTransform[0][2], 1e-6);
            EXPECT_NEAR(transform.getMat()[1][0], expectedTransform[1][0], 1e-6);
            EXPECT_NEAR(transform.getMat()[1][1], expectedTransform[1][1], 1e-6);
            EXPECT_NEAR(transform.getMat()[1][2], expectedTransform[1][2], 1e-6);
            EXPECT_NEAR(transform.getMat()[2][0], expectedTransform[2][0], 1e-6);
            EXPECT_NEAR(transform.getMat()[2][1], expectedTransform[2][1], 1e-6);
            EXPECT_NEAR(transform.getMat()[2][2], expectedTransform[2][2], 1e-6);

            Vec2f mappedPoint = transform.mapPoint(point);
            Vec2f expectedMappedPoint(-4.0f * 4.0f + 1.0f, 3.0f * 3.0f + 2.0f);
            EXPECT_NEAR(mappedPoint.x, expectedMappedPoint.x, 1e-6);
            EXPECT_NEAR(mappedPoint.y, expectedMappedPoint.y, 1e-6);

            Vec2f mappedVector = transform.mapVector(vector);
            Vec2f expectedMappedVector(-4.0f * 4.0f, 3.0f * 3.0f);
            EXPECT_NEAR(mappedVector.x, expectedMappedVector.x, 1e-6);
            EXPECT_NEAR(mappedVector.y, expectedMappedVector.y, 1e-6);
        }
    }

    TEST(Transform2D, inv) {
        Transform2D transform(Vec2f(1.0f, 2.0f), Deg(90.0f), Vec2f(3.0f, 4.0f));
        Transform2D inverseTransform = transform.inv();
        Mat3f expectedInverse(
                0.0f, 1.0f / 3.0f, -2.0f / 3.0f,
                -0.25f, 0.0f, 0.25f,
                0.0f, 0.0f, 1.0f);
        EXPECT_NEAR(inverseTransform.getMat()[0][0], expectedInverse[0][0], 1e-6);
        EXPECT_NEAR(inverseTransform.getMat()[0][1], expectedInverse[0][1], 1e-6);
        EXPECT_NEAR(inverseTransform.getMat()[0][2], expectedInverse[0][2], 1e-6);
        EXPECT_NEAR(inverseTransform.getMat()[1][0], expectedInverse[1][0], 1e-6);
        EXPECT_NEAR(inverseTransform.getMat()[1][1], expectedInverse[1][1], 1e-6);
        EXPECT_NEAR(inverseTransform.getMat()[1][2], expectedInverse[1][2], 1e-6);
        EXPECT_NEAR(inverseTransform.getMat()[2][0], expectedInverse[2][0], 1e-6);
        EXPECT_NEAR(inverseTransform.getMat()[2][1], expectedInverse[2][1], 1e-6);
        EXPECT_NEAR(inverseTransform.getMat()[2][2], expectedInverse[2][2], 1e-6);
    }
}

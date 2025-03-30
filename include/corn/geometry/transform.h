#pragma once

#include <corn/geometry/deg.h>
#include <corn/geometry/mat.h>
#include <corn/geometry/rotation.h>
#include <corn/geometry/vec.h>

namespace corn {
    /**
     * @class Transform2D
     * @brief Represents a 3x3 transformation matrix.
     *
     * The transformation matrix is used to transform points and vectors in 2D space.
     * To transform a point Vec2f(x, y) with matrix M:
     *     M * Vec3f(x, y, 1).
     * To transform a vector Vec2f(x, y) with matrix M:
     *     M * Vec3f(x, y, 0).
     * Or simply just call `mapPoint` or `mapVector` methods.
     */
    class Transform2D {
    public:
        /// @brief Constructor.
        Transform2D() noexcept;

        /// @brief Constructor.
        Transform2D(const Vec2f& translation, const Deg& rotation, const Vec2f& dilation) noexcept;

        /// @brief Constructor.
        explicit Transform2D(Mat3f mat);

        /// @return The identity 2D transform.
        [[nodiscard]] static const Transform2D& I() noexcept;

        /**
         * @param translation Translation vector.
         * @return A 2D transform that translates by the given vector.
         */
        [[nodiscard]] static Transform2D translate(const Vec2f& translation) noexcept;

        /**
         * @param rotation Rotation angle in degrees.
         * @return A 2D transform that rotates by the given angle.
         */
        [[nodiscard]] static Transform2D rotate(const Deg& rotation) noexcept;

        /**
         * @param dilation Dilation vector.
         * @return A 2D transform that dilates by the given vector.
         */
        [[nodiscard]] static Transform2D dilate(const Vec2f& dilation) noexcept;

        /**
         * @param point Point to be transformed.
         * @return Transformed point.
         */
        [[nodiscard]] Vec2f mapPoint(const Vec2f& point) const noexcept;

        /**
         * @param vec Vector to be transformed.
         * @return Transformed vector.
         *
         * Vectors are not affected by translation.
         */
        [[nodiscard]] Vec2f mapVector(const Vec2f& vec) const noexcept;

        /**
         * @return The inverse of the transformation matrix.
         *
         * If the transformation matrix is singular, returns the zero matrix.
         */
        [[nodiscard]] Transform2D inv() const noexcept;

        /// @return The transformation matrix.
        [[nodiscard]] const Mat3f& getMat() const noexcept;

        /// @return The translational component of the transformation matrix.
        [[nodiscard]] Vec2f getTranslationComponent() const noexcept;

        /// @return The rotational component of the transformation matrix.
        [[nodiscard]] Deg getRotationComponent() const noexcept;

        /// @return The dilational component of the transformation matrix.
        [[nodiscard]] Mat2f getDilationComponent() const noexcept;

    private:
        Mat3f mat_;
    };

    /**
     * @class Transform3D
     * @brief Represents a 4x4 transformation matrix.
     *
     * The transformation matrix is used to transform points and vectors in 3D space.
     * To transform a point Vec3f(x, y, z) with matrix M:
     *     M * Vec4f(x, y, z, 1).
     * To transform a vector Vec3f(x, y, z) with matrix M:
     *     M * Vec4f(x, y, z, 0).
     * Or simply just call `mapPoint` or `mapVector` methods.
     */
    class Transform3D {
    public:
        /// @brief Constructor.
        Transform3D() noexcept;

        /// @brief Constructor. The quaternion is assumed to be normalized.
        Transform3D(const Vec3f& translation, const Quaternion& rotation, const Vec3f& dilation) noexcept;

        /// @brief Constructor.
        explicit Transform3D(Mat4f mat);

        /// @return The identity 3D transform.
        [[nodiscard]] static const Transform3D& I() noexcept;

        /**
         * @param translation Translation vector.
         * @return A 3D transform that translates by the given vector.
         */
        [[nodiscard]] static Transform3D translate(const Vec3f& translation) noexcept;

        /**
         * @param rotation Rotation quaternion.
         * @return A 3D transform that rotates by the given quaternion.
         */
        [[nodiscard]] static Transform3D rotate(const Quaternion& rotation) noexcept;

        /**
         * @param dilation Dilation vector.
         * @return A 3D transform that dilates by the given vector.
         */
        [[nodiscard]] static Transform3D dilate(const Vec3f& dilation) noexcept;

        /**
         * @param point Point to be transformed.
         * @return Transformed point.
         */
        [[nodiscard]] Vec3f mapPoint(const Vec3f& point) const noexcept;

        /**
         * @param vec Vector to be transformed.
         * @return Transformed vector.
         *
         * Vectors are not affected by translation.
         */
        [[nodiscard]] Vec3f mapVector(const Vec3f& vec) const noexcept;

        /// @return The transformation matrix.
        [[nodiscard]] const Mat4f& getMat() const noexcept;

        /// @return The translational component of the transformation matrix.
        [[nodiscard]] Vec3f getTranslationComponent() const noexcept;

        /// @return The rotational component of the transformation matrix.
        [[nodiscard]] Quaternion getRotationComponent() const noexcept;

        /// @return The dilational component of the transformation matrix.
        [[nodiscard]] Mat3f getDilationComponent() const noexcept;

    private:
        Mat4f mat_;
    };

    /**
     * @param transform The 2D transformation matrix.
     * @param vec The vector.
     * @return Product of the 2D transformation matrix and the vector.
     */
    [[nodiscard]] Vec3f operator*(const Transform2D& transform, const Vec3f& vec) noexcept;

    /**
     * @param transform The 3D transformation matrix.
     * @param vec The vector.
     * @return Product of the 3D transformation matrix and the vector.
     */
    [[nodiscard]] Vec4f operator*(const Transform3D& transform, const Vec4f& vec) noexcept;

    /**
     * @param lhs The first 2D transformation matrix.
     * @param rhs The second 2D transformation matrix.
     * @return Product of the two transformation matrices.
     *
     * The product of two transformation matrices is a new transformation matrix that represents the combined
     * transformation.
     */
    [[nodiscard]] Transform2D operator*(const Transform2D& lhs, const Transform2D& rhs) noexcept;

    /**
     * @param lhs The first 3D transformation matrix.
     * @param rhs The second 3D transformation matrix.
     * @return Product of the two 3D transformation matrices.
     *
     * The product of two transformation matrices is a new transformation matrix that represents the combined
     * transformation.
     */
    [[nodiscard]] Transform3D operator*(const Transform3D& lhs, const Transform3D& rhs) noexcept;
}

#include <corn/geometry/transform.h>
#include <corn/geometry/operations.h>
#include <corn/util/constants.h>

namespace corn {
    Transform2D::Transform2D() noexcept
            : mat_{ Vec3f::E<0>(), Vec3f::E<1>(), Vec3f::E<2>() } {}

    Transform2D::Transform2D(const Vec2f& translation, const Deg& rotation, const Vec2f& dilation) noexcept {
        float cosDeg = rotation.cos();
        float sinDeg = rotation.sin();
        this->mat_ = Mat3f(
                dilation.x * cosDeg, -dilation.y * sinDeg, translation.x,
                dilation.x * sinDeg, dilation.y * cosDeg, translation.y,
                0.0f, 0.0f, 1.0f);
    }

    Transform2D::Transform2D(Mat3f mat) noexcept
            : mat_(std::move(mat)) {}

    const Transform2D& Transform2D::I() noexcept {
        static const Transform2D identity;
        return identity;
    }

    Transform2D Transform2D::translate(const Vec2f& translation) noexcept {
        return { translation, Deg(0.0f), Vec2f(1.0f, 1.0f) };
    }

    Transform2D Transform2D::rotate(const Deg& rotation) noexcept {
        return { Vec2f::O(), rotation, Vec2f(1.0f, 1.0f) };
    }

    Transform2D Transform2D::dilate(const Vec2f& dilation) noexcept {
        return { Vec2f::O(), Deg(0.0f), dilation };
    }

    Vec2f Transform2D::mapPoint(const Vec2f& point) const noexcept {
        return (*this * point.append<1>(1.0f)).to<2>();
    }

    Vec2f Transform2D::mapVector(const Vec2f& vec) const noexcept {
        return (*this * vec.append<1>(0.0f)).to<2>();
    }

    Transform2D Transform2D::inv() const noexcept {
        return Transform2D(this->mat_.inv());
    }

    const Mat3f& Transform2D::getMat() const noexcept {
        return this->mat_;
    }

    Vec2f Transform2D::getTranslationComponent() const noexcept {
        return Vec2f(this->mat_[0][2], this->mat_[1][2]);
    }

    Deg Transform2D::getRotationComponent() const noexcept {
        // TODO: Consider non-similarity transformations
        return atan2(this->mat_[1][0], this->mat_[0][0]) * 180.0f / (float)PI;
    }

    Vec2f Transform2D::getDilationComponent() const noexcept {
        return Vec2f(this->mat_.col(0).norm(), this->mat_.col(1).norm());
    }

    Transform3D::Transform3D() noexcept
            : mat_{ Mat4f::I() } {}

    Transform3D::Transform3D(const Vec3f& translation, const Quaternion& rotation, const Vec3f& dilation) noexcept {
        this->mat_ = (
                Transform3D::translate(translation) *
                Transform3D::rotate(rotation) *
                Transform3D::dilate(dilation)
        ).getMat();
    }

    Transform3D::Transform3D(Mat4f mat) noexcept
            : mat_(std::move(mat)) {}

    const Transform3D& Transform3D::I() noexcept {
        static const Transform3D identity;
        return identity;
    }

    Transform3D Transform3D::translate(const Vec3f& translation) noexcept {
        return Transform3D(Mat4f(
                1.0f, 0.0f, 0.0f, translation.x,
                0.0f, 1.0f, 0.0f, translation.y,
                0.0f, 0.0f, 1.0f, translation.z,
                0.0f, 0.0f, 0.0f, 1.0f));
    }

    Transform3D Transform3D::rotate(const Quaternion& rotation) noexcept {
        Vec3f rotatedX = corn::rotate(Vec3f::E<0>(), rotation);
        Vec3f rotatedY = corn::rotate(Vec3f::E<1>(), rotation);
        Vec3f rotatedZ = corn::rotate(Vec3f::E<2>(), rotation);
        return Transform3D(Mat4f(
                Vec4f(rotatedX.x, rotatedY.x, rotatedZ.x, 0.0f),
                Vec4f(rotatedX.y, rotatedY.y, rotatedZ.y, 0.0f),
                Vec4f(rotatedX.z, rotatedY.z, rotatedZ.z, 0.0f),
                Vec4f(0.0f, 0.0f, 0.0f, 1.0f)));
    }

    Transform3D Transform3D::dilate(const Vec3f& dilation) noexcept {
        return Transform3D(Mat4f(
                Vec4f(dilation.x, 0.0f, 0.0f, 0.0f),
                Vec4f(0.0f, dilation.y, 0.0f, 0.0f),
                Vec4f(0.0f, 0.0f, dilation.z, 0.0f),
                Vec4f(0.0f, 0.0f, 0.0f, 1.0f)));
    }

    Vec3f Transform3D::mapPoint(const Vec3f& point) const noexcept {
        return (*this * point.append<1>(1.0f)).to<3>();
    }

    Vec3f Transform3D::mapVector(const Vec3f& vec) const noexcept {
        return (*this * vec.append<1>(0.0f)).to<3>();
    }

    const Mat4f& Transform3D::getMat() const noexcept {
        return this->mat_;
    }

    Vec3f Transform3D::getTranslationComponent() const noexcept {
        return this->mat_[3].to<3>();
    }

    Quaternion Transform3D::getRotationComponent() const noexcept {
        // TODO: Consider non-similarity transformations
        // Find the rotation matrix R
        std::array<Vec3f, 3> R {
                this->mat_[0].to<3>().normalize(),
                this->mat_[1].to<3>().normalize(),
                this->mat_[2].to<3>().normalize()
        };

        float trace = R[0][0] + R[1][1] + R[2][2];
        float x, y, z, w;

        // Since we are using column-major order, we need to swap the indices
        if (trace > 0.0f) {
            float s = std::sqrt(trace + 1.0f) * 2.0f; // s = 4*w
            w = 0.25f * s;
            x = (R[1][2] - R[2][1]) / s;
            y = (R[2][0] - R[0][2]) / s;
            z = (R[0][1] - R[1][0]) / s;
        } else if (R[0][0] > R[1][1] && R[0][0] > R[2][2]) {
            float s = std::sqrt(1.0f + R[0][0] - R[1][1] - R[2][2]) * 2.0f;
            w = (R[1][2] - R[2][1]) / s;
            x = 0.25f * s;
            y = (R[0][1] + R[1][0]) / s;
            z = (R[2][0] + R[0][2]) / s;
        } else if (R[1][1] > R[2][2]) {
            float s = std::sqrt(1.0f + R[1][1] - R[0][0] - R[2][2]) * 2.0f;
            w = (R[2][0] - R[0][2]) / s;
            x = (R[0][1] + R[1][0]) / s;
            y = 0.25f * s;
            z = (R[1][2] + R[2][1]) / s;
        } else {
            float s = std::sqrt(1.0f + R[2][2] - R[0][0] - R[1][1]) * 2.0f;
            w = (R[0][1] - R[1][0]) / s;
            x = (R[2][0] + R[0][2]) / s;
            y = (R[1][2] + R[2][1]) / s;
            z = 0.25f * s;
        }

        // Return the normalized quaternion
        return Quaternion(x, y, z, w).normalize();
    }

    Vec3f Transform3D::getDilationComponent() const noexcept {
        return Vec3f(this->mat_[0].norm(), this->mat_[1].norm(), this->mat_[2].norm());
    }

    Vec3f operator*(const Transform2D& transform, const Vec3f& vec) noexcept {
        return transform.getMat() * vec;
    }

    Vec4f operator*(const Transform3D& transform, const Vec4f& vec) noexcept {
        return transform.getMat() * vec;
    }

    Transform2D operator*(const Transform2D& lhs, const Transform2D& rhs) noexcept {
        return Transform2D(lhs.getMat() * rhs.getMat());
    }

    Transform3D operator*(const Transform3D& lhs, const Transform3D& rhs) noexcept {
        return Transform3D(lhs.getMat() * rhs.getMat());
    }
}

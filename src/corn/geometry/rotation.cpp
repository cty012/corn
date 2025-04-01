#include <corn/geometry/rotation.h>
#include <corn/geometry/vec.h>

namespace corn {
    Quaternion EulerAngles::quat() const noexcept {
        return Quaternion(this->yaw, Vec3f::E_Y())
            * Quaternion(this->pitch, Vec3f::E_X())
            * Quaternion(this->roll, Vec3f::E_Z());
    }

    Quaternion::Quaternion(Vec4f val) noexcept : val(std::move(val)) {}

    Quaternion::Quaternion(float a, float b, float c, float d) noexcept : val(a, b, c, d) {}

    Quaternion::Quaternion(const Deg& theta, const Vec3f& axis) noexcept {
        Vec3f normalizedAxis = axis.normalize();
        Deg temp = theta * 0.5;
        float a = temp.cos();
        float a_ = temp.sin();
        this->val = (normalizedAxis * a_).append<1>(a);
    }

    const Quaternion& Quaternion::I() noexcept {
        static const Quaternion identity(0.0f, 0.0f, 0.0f, 1.0f);
        return identity;
    }

    float Quaternion::norm() const noexcept {
        return this->val.norm();
    }

    Quaternion Quaternion::normalize() const noexcept {
        return Quaternion(this->val.normalize());
    }

    Quaternion Quaternion::inv() const noexcept {
        return { this->val.x, this->val.y, this->val.z, -this->val.w };
    }

    Quaternion operator+(const Quaternion& rhs) noexcept {
        return rhs;
    }

    Quaternion operator-(const Quaternion& rhs) noexcept {
        return Quaternion(-rhs.val);
    }

    Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) noexcept {
        return Quaternion(lhs.val + rhs.val);
    }

    Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs) noexcept {
        return Quaternion(lhs.val - rhs.val);
    }

    Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) noexcept {
        return {
                lhs.val.w * rhs.val.x + lhs.val.x * rhs.val.w + lhs.val.y * rhs.val.z - lhs.val.z * rhs.val.y,
                lhs.val.w * rhs.val.y - lhs.val.x * rhs.val.z + lhs.val.y * rhs.val.w + lhs.val.z * rhs.val.x,
                lhs.val.w * rhs.val.z + lhs.val.x * rhs.val.y - lhs.val.y * rhs.val.x + lhs.val.z * rhs.val.w,
                lhs.val.w * rhs.val.w - lhs.val.x * rhs.val.x - lhs.val.y * rhs.val.y - lhs.val.z * rhs.val.z,
        };
    }

    Quaternion& operator+=(Quaternion& lhs, const Quaternion& rhs) noexcept {
        lhs.val += rhs.val;
        return lhs;
    }

    Quaternion& operator-=(Quaternion& lhs, const Quaternion& rhs) noexcept {
        lhs.val -= rhs.val;
        return lhs;
    }

    Quaternion& operator*=(Quaternion& lhs, const Quaternion& rhs) noexcept {
        lhs = lhs * rhs;
        return lhs;
    }
}

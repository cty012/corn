#include <corn/geometry/rotation.h>
#include <corn/geometry/vec3.h>

namespace corn {
    Quaternion EulerAngles::quat() const noexcept {
        return Quaternion(this->yaw, Vec3::UNIT_Y())
            * Quaternion(this->pitch, Vec3::UNIT_X())
            * Quaternion(this->roll, Vec3::UNIT_Z());
    }

    Quaternion::Quaternion(Vec4 val) noexcept : val(val) {}

    Quaternion::Quaternion(float a, float b, float c, float d) noexcept : val(a, b, c, d) {}

    Quaternion::Quaternion(const Deg& theta, const Vec3& axis) noexcept {
        Vec3 normalizedAxis = axis.normalize();
        Deg temp = theta * 0.5;
        float a = temp.cos();
        float a_ = temp.sin();
        val = { a, a_ * normalizedAxis.x, a_ * normalizedAxis.y, a_ * normalizedAxis.z };
    }

    float Quaternion::norm() const noexcept {
        return this->val.norm();
    }

    Quaternion Quaternion::normalize() const noexcept {
        return this->val.normalize();
    }

    Quaternion Quaternion::inv() const noexcept {
        return { -this->val.x, this->val.y, this->val.z, this->val.w };
    }

    Quaternion operator+(const Quaternion& rhs) noexcept {
        return rhs;
    }

    Quaternion operator-(const Quaternion& rhs) noexcept {
        return -rhs.val;
    }

    Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) noexcept {
        return lhs.val + rhs.val;
    }

    Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs) noexcept {
        return lhs.val - rhs.val;
    }

    Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) noexcept {
        return {
                lhs.val.x * rhs.val.x - lhs.val.y * rhs.val.y - lhs.val.z * rhs.val.z - lhs.val.w * rhs.val.w,
                lhs.val.x * rhs.val.y + lhs.val.y * rhs.val.x + lhs.val.z * rhs.val.w - lhs.val.w * rhs.val.z,
                lhs.val.x * rhs.val.z - lhs.val.y * rhs.val.w + lhs.val.z * rhs.val.x + lhs.val.w * rhs.val.y,
                lhs.val.x * rhs.val.w + lhs.val.y * rhs.val.z - lhs.val.z * rhs.val.y + lhs.val.w * rhs.val.x,
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
        float x = lhs.val.x * rhs.val.x - lhs.val.y * rhs.val.y - lhs.val.z * rhs.val.z - lhs.val.w * rhs.val.w;
        float y = lhs.val.x * rhs.val.y + lhs.val.y * rhs.val.x + lhs.val.z * rhs.val.w - lhs.val.w * rhs.val.z;
        float z = lhs.val.x * rhs.val.z - lhs.val.y * rhs.val.w + lhs.val.z * rhs.val.x + lhs.val.w * rhs.val.y;
        float w = lhs.val.x * rhs.val.w + lhs.val.y * rhs.val.z - lhs.val.z * rhs.val.y + lhs.val.w * rhs.val.x;
        lhs.val.x = x;
        lhs.val.y = y;
        lhs.val.z = z;
        lhs.val.w = w;
        return lhs;
    }
}

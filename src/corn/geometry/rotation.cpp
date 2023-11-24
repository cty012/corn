#include <cmath>
#include <corn/geometry/rotation.h>

namespace corn {
    Quaternion EulerAngles::quat() const {
        return Quaternion(this->yaw, Vec3::UNIT_Y())
            * Quaternion(this->pitch, Vec3::UNIT_X())
            * Quaternion(this->roll, Vec3::UNIT_Z());
    }

    Quaternion::Quaternion(Vec4 val) : val(val) {}

    Quaternion::Quaternion(float a, float b, float c, float d) : val(a, b, c, d) {}

    Quaternion::Quaternion(const Deg& theta, const Vec3& axis) {
        Vec3 normalizedAxis = axis.normalize();
        Deg temp = theta * 0.5;
        float a = temp.cos();
        float a_ = temp.sin();
        val = { a, a_ * normalizedAxis.x, a_ * normalizedAxis.y, a_ * normalizedAxis.z };
    }

    float Quaternion::norm() const {
        return this->val.norm();
    }

    Quaternion Quaternion::normalize() const {
        return this->val.normalize();
    }

    Quaternion Quaternion::inv() const {
        return { -this->val.x, this->val.y, this->val.z, this->val.w };
    }

    Quaternion operator+(const Quaternion& rhs) {
        return rhs;
    }

    Quaternion operator-(const Quaternion& rhs) {
        return -rhs.val;
    }

    Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) {
        return lhs.val + rhs.val;
    }

    Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs) {
        return lhs.val - rhs.val;
    }

    Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) {
        return {
                lhs.val.x * rhs.val.x - lhs.val.y * rhs.val.y - lhs.val.z * rhs.val.z - lhs.val.w * rhs.val.w,
                lhs.val.x * rhs.val.y + lhs.val.y * rhs.val.x + lhs.val.z * rhs.val.w - lhs.val.w * rhs.val.z,
                lhs.val.x * rhs.val.z - lhs.val.y * rhs.val.w + lhs.val.z * rhs.val.x + lhs.val.w * rhs.val.y,
                lhs.val.x * rhs.val.w + lhs.val.y * rhs.val.z - lhs.val.z * rhs.val.y + lhs.val.w * rhs.val.x,
        };
    }

    Quaternion& operator+=(Quaternion& lhs, const Quaternion& rhs) {
        lhs.val += rhs.val;
        return lhs;
    }

    Quaternion& operator-=(Quaternion& lhs, const Quaternion& rhs) {
        lhs.val -= rhs.val;
        return lhs;
    }

    Quaternion& operator*=(Quaternion& lhs, const Quaternion& rhs) {
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

#include <cmath>
#include <corn/geometry/rotation.h>

namespace corn {
    Quaternion EulerAngles::quat() const {
        return Quaternion(this->yaw, Vec3::UNIT_Y)
            * Quaternion(this->pitch, Vec3::UNIT_X)
            * Quaternion(this->roll, Vec3::UNIT_Z);
        // float cyaw = this->yaw.cos();
        // float syaw = this->yaw.sin();
        // float cpitch = this->yaw.cos();
        // float spitch = this->yaw.sin();
        // float croll = this->yaw.cos();
        // float sroll = this->yaw.sin();
        // return {
        //     cyaw * cpitch * croll + syaw * spitch * sroll,
        //     cyaw * spitch * croll + syaw * cpitch * sroll,
        //     -cyaw * spitch * sroll + syaw * cpitch * croll,
        //     cyaw * cpitch * sroll - syaw * spitch * croll,
        // };
    }

    Quaternion::Quaternion(Vec4 val): val(val) {}

    Quaternion::Quaternion(float a, float b, float c, float d): val(a, b, c, d) {}

    Quaternion::Quaternion(const Deg& theta, const Vec3& axis) {
        Vec3 normalizedAxis = axis.normalize();
        Deg temp = theta.mult(0.5);
        float a = temp.cos();
        float a_ = temp.sin();
        val = {a, a_ * normalizedAxis.x, a_ * normalizedAxis.y, a_ * normalizedAxis.z};
    }

    Vec3 Quaternion::transform(const Vec3& point) const {
        Quaternion p = {0, point.x, point.y, point.z};
        Vec4 result = ((*this) * p * this->inv()).val;
        return {result.y, result.z, result.w};  // NOLINT
    }

    Quaternion Quaternion::operator+() const {
        return *this;
    }

    Quaternion Quaternion::operator-() const {
        return -this->val;
    }

    Quaternion Quaternion::operator+(const Quaternion& other) const {
        return this->val + other.val;
    }

    Quaternion Quaternion::operator-(const Quaternion& other) const {
        return this->val - other.val;
    }

    Quaternion& Quaternion::operator+=(const Quaternion& other) {
        this->val += other.val;
        return *this;
    }

    Quaternion& Quaternion::operator-=(const Quaternion& other) {
        this->val -= other.val;
        return *this;
    }

    Quaternion Quaternion::operator*(const Quaternion& other) const {
        const Vec4& o = other.val;
        return {
            val.x * o.x - val.y * o.y - val.z * o.z - val.w * o.w,
            val.x * o.y + val.y * o.x + val.z * o.w - val.w * o.z,
            val.x * o.z - val.y * o.w + val.z * o.x + val.w * o.y,
            val.x * o.w + val.y * o.z - val.z * o.y + val.w * o.x,
        };
    }

    Quaternion& Quaternion::operator*=(const Quaternion& other) {
        const Vec4& o = other.val;
        float x = val.x * o.x - val.y * o.y - val.z * o.z - val.w * o.w;
        float y = val.x * o.y + val.y * o.x + val.z * o.w - val.w * o.z;
        float z = val.x * o.z - val.y * o.w + val.z * o.x + val.w * o.y;
        float w = val.x * o.w + val.y * o.z - val.z * o.y + val.w * o.x;
        val.x = x;
        val.y = y;
        val.z = z;
        val.w = w;
        return *this;
    }

    float Quaternion::norm() const {
        return this->val.norm();
    }

    Quaternion Quaternion::normalize() const {
        return this->val.normalize();
    }

    Quaternion Quaternion::inv() const {
        return {-this->val.x, this->val.y, this->val.z, this->val.w};
    }
}

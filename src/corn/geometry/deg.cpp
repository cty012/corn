#include <cmath>
#include <corn/geometry/deg.h>
#include <corn/util/constants.h>

namespace corn {
    Deg::Deg(float val) noexcept : val_(0.0) {
        this->set(val);
    }

    float Deg::get() const noexcept {
        return this->val_;
    }

    void Deg::set(float val) noexcept {
        val = fmodf(val, 360.0);
        this->val_ = val >= 0 ? val : val + 360.0f;
    }

    float Deg::sin() const noexcept {
        static constexpr auto degToRad = (float)(PI / 180.0);
        return std::sin(this->val_ * degToRad);
    }

    float Deg::cos() const noexcept {
        static constexpr auto degToRad = (float)(PI / 180.0);
        return std::cos(this->val_ * degToRad);
    }

    Deg operator+(const Deg& rhs) noexcept {
        return rhs;
    }

    Deg operator-(const Deg& rhs) noexcept {
        return -rhs.get();
    }

    Deg operator+(const Deg& lhs, const Deg& rhs) noexcept {
        return lhs.get() + rhs.get();
    }

    Deg operator-(const Deg& lhs, const Deg& rhs) noexcept {
        return lhs.get() - rhs.get();
    }

    Deg operator*(const Deg& deg, float scalar) noexcept {
        return deg.get() * scalar;
    }

    Deg operator*(float scalar, const Deg& deg) noexcept {
        return scalar * deg.get();
    }

    Deg& operator+=(Deg& lhs, const Deg& rhs) noexcept {
        lhs.set(lhs.get() + rhs.get());
        return lhs;
    }

    Deg& operator-=(Deg& lhs, const Deg& rhs) noexcept {
        lhs.set(lhs.get() - rhs.get());
        return lhs;
    }
}

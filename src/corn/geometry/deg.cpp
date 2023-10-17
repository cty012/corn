#include <cmath>
#include <corn/geometry/deg.h>

namespace corn {
    Deg::Deg(float val): _val(0.0) {
        this->set(val);
    }

    float Deg::get() const {
        return this->_val;
    }

    void Deg::set(float val) {
        val = fmodf(val, 360.0);
        this->_val = val >= 0 ? val : val + 360.0f;
    }

    Deg Deg::operator+() const {
        return *this;
    }

    Deg Deg::operator-() const {
        return -this->_val;
    }

    Deg Deg::operator+(const Deg& other) const {
        return this->_val + other._val;
    }

    Deg Deg::operator-(const Deg& other) const {
        return this->_val - other._val;
    }

    Deg& Deg::operator+=(const Deg& other) {
        this->set(this->_val + other._val);
        return *this;
    }

    Deg& Deg::operator-=(const Deg& other) {
        this->set(this->_val - other._val);
        return *this;
    }

    Deg Deg::mult(float factor) const {
        return this->_val * factor;
    }

    float Deg::sin() const {
        static constexpr float degToRad = M_PI / 180;
        return std::sin(this->_val * degToRad);
    }

    float Deg::cos() const {
        static constexpr float degToRad = M_PI / 180;
        return std::cos(this->_val * degToRad);
    }
}

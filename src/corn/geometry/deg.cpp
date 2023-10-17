#include <cmath>
#include <corn/geometry/deg.h>

namespace corn {
    Deg::Deg(double val): _val(0.0) {
        this->set(val);
    }

    double Deg::get() const {
        return _val;
    }

    void Deg::set(double val) {
        val = std::fmod(val, 360.0);
        _val = val >= 0 ? val : val + 360.0;
    }

    Deg Deg::operator+() const {
        return *this;
    }

    Deg Deg::operator-() const {
        return -_val;
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

    Deg Deg::mult(double factor) const {
        return this->_val * factor;
    }

    double Deg::sin() const {
        return std::sin(this->_val);
    }

    double Deg::cos() const {
        return std::cos(this->_val);
    }
}

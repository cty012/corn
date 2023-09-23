#include <cmath>
#include <corn/util/geometry.h>

namespace corn {
    Deg::Deg(double val) {
        val = std::fmod(val, 360.0);
        _val = val >= 0 ? val : val + 360.0;
    }

    // TODO
}

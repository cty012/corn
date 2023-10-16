#pragma once

namespace corn {
    /**
     * @class Deg
     * @brief double in range [0, 360).
     *
     * @todo Implement this struct.
     */
    struct Deg {
        Deg(double val = 0.0);  // NOLINT
        // TODO: operator overload
    private:
        double _val;
    };
}

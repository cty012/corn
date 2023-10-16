#pragma once

#include <corn/geometry/vec2.h>
#include <corn/geometry/vec4.h>

namespace corn {
    /**
     * @class ReferenceFrame
     * @brief A reference frame is a rectangle. All points in the reference frame are defined with respect to the four
     * sides of the rectangle.
     *
     * Suppose the reference frame has values {l, t, r, b}. A point with relative location <x, y, z, w> will be mapped
     * to the absolute location at <(1 - z)l + zr, (1 - w)t + wb> + <x, y>.
     *
     * Note that the absolute location may not lie inside the rectangle.
     *
     * For example,
     * 1. To create a point with distance 40 to the left side and 30 to the bottom, the relative location
     * should be <40, -30, 0, 1>.
     * 2. To create a point 1/3 to the left side and on the top edge, the relative location should be
     * <0, 0, 1.0/3.0, 0>.
     * 3. To create a point at the center, the relative location should be <0, 0, 0.5, 0.5>.
     */
    struct ReferenceFrame {
        double left;
        double top;
        double right;
        double bottom;

        ReferenceFrame(double left, double top, double right, double bottom);
        [[nodiscard]] Vec2 map(const Vec4& relLocation) const;
    };
}

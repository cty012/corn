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
     * Examples of relative location:
     * 1. To create a point with distance 40 to the left side and 30 to the bottom, the relative location
     * should be <40, -30, 0, 1>.
     * 2. To create a point 1/3 to the left side and on the top edge, the relative location should be
     * <0, 0, 1.0/3.0, 0>.
     * 3. To create a point at the center, the relative location should be <0, 0, 0.5, 0.5>.
     *
     * @see Vec2
     * @see Vec4
     * @see RelativeFrame
     */
    struct ReferenceFrame {
        double left;
        double top;
        double right;
        double bottom;

        /// @brief Converts the relative location into an absolute location.
        [[nodiscard]] Vec2 map(const Vec4& relativeLocation) const;
    };

    /**
     * @class RelativeFrame
     * @brief A relative frame is a map between reference frames. In fact, it defines the child reference frame relative
     * to the parent reference frame by specifying the relative location of the top-left and bottom-right corners.
     *
     * Suppose the parent reference frame is parentFrame, the top-left corner's relative location is <x1, y1, z1, w1>,
     * and the bottom-right corner's relative location is <x2, y2, z2, w2>. Then the child's reference frame will be
     * <l', t', r', b'> where <l', t'> = parentFrame.map(<x1, y1, z1, w1>) and
     * <r', b'> = parentFrame.map(<x2, y2, z2, w2>).
     *
     * Note that the child reference frame may not lie inside the parent reference frame.
     *
     * @see Vec4
     * @see ReferenceFrame
     */
    struct RelativeFrame {
        Vec4 topLeft;
        Vec4 bottomRight;

        /**
         * @brief Initialize with the relative location of the top-left and bottom-right corner with respect to the
         * parent's reference frame.
         */
        RelativeFrame(const Vec4& topLeft, const Vec4& bottomRight);

        /// @brief Converts the parent's reference frame into the child's reference frame.
        [[nodiscard]] ReferenceFrame map(const ReferenceFrame& parentFrame) const;
    };
}

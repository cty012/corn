#pragma once

#import <Foundation/Foundation.h>
#include <corn/geometry/transform.h>
#include <corn/geometry/vec.h>

namespace corn {
    CGFloat cssWeightToCTFontWeight(float weight);

    CGAffineTransform getCGAffineTransform(const Transform2D& transform);

    /**
     * @brief Calculates the bounding box after applying a transform to a rectangle with the given size.
     * @param transform The transform to apply.
     * @param size The size of the rectangle.
     * @return The bounding box [top-left-x, top-left-y, width, height].
     *
     * The four corners of the rectangle are:
     *   - top-left: (0, 0)
     *   - top-right: (size.x, 0)
     *   - bottom-left: (0, size.y)
     *   - bottom-right: (size.x, size.y)
     * The bounding box is the minimum rectangle (with integer coordinates) that contains all four corners after
     * applying the transform to the corners.
     */
    Vec<int16_t, 4> calcBoundingBox(const Transform2D& transform, const Vec2f& size);
}

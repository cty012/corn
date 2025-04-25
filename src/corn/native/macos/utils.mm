#include "macos/utils.h"

namespace corn {
    CGFloat cssWeightToCTFontWeight(float weight) {
        // CSS font weight vs CTFont weight:
        // 100: -1.0
        // 200: -0.8
        // 300: -0.6
        // 400: -0.0
        // 500: 0.23
        // 600: 0.3
        // 700: 0.4
        // 800: 0.56
        // 900: 0.62
        if (weight <= 100) {
            return -1.0f;
        } else if (weight <= 200) {
            return -1.0f + 0.2f * (weight - 100) / 100.0f;
        } else if (weight <= 300) {
            return -0.8f + 0.2f * (weight - 200) / 100.0f;
        } else if (weight <= 400) {
            return -0.6f + 0.6f * (weight - 300) / 100.0f;
        } else if (weight <= 500) {
            return 0.0f + 0.23f * (weight - 400) / 100.0f;
        } else if (weight <= 600) {
            return 0.23f + 0.07f * (weight - 500) / 100.0f;
        } else if (weight <= 700) {
            return 0.3f + 0.10f * (weight - 600) / 100.0f;
        } else if (weight <= 800) {
            return 0.4f + 0.16f * (weight - 700) / 100.0f;
        } else if (weight <= 900) {
            return 0.56f + 0.06f * (weight - 800) / 100.0f;
        } else {
            return 0.62f;  // Cap at 0.62
        }
        return 2.0f;
    }

    CGAffineTransform getCGAffineTransform(const Transform2D& transform) {
        const Mat3f& mat = transform.getMat();
        return {
                .a = mat[0][0],
                .b = mat[1][0],
                .c = mat[0][1],
                .d = mat[1][1],
                .tx = mat[0][2],
                .ty = mat[1][2],
        };
    }

    Vec<int16_t, 4> calcBoundingBox(const Transform2D& transform, const Vec2f& size) {
        /// Determine the typographic bounds.
        // Apply transform to the text to find the actual width and height.
        Vec2f ul = transform.mapPoint(Vec2f(0, size.y));
        Vec2f ur = transform.mapPoint(size);
        Vec2f bl = transform.mapPoint(Vec2f::O());
        Vec2f br = transform.mapPoint(Vec2f(size.x, 0));

        auto minX = static_cast<int16_t>(std::floor(std::fmin(std::fmin(ul.x, ur.x), std::fmin(bl.x, br.x))));
        auto minY = static_cast<int16_t>(std::floor(std::fmin(std::fmin(ul.y, ur.y), std::fmin(bl.y, br.y))));
        auto maxX = static_cast<int16_t>(std::ceil(std::fmax(std::fmax(ul.x, ur.x), std::fmax(bl.x, br.x))));
        auto maxY = static_cast<int16_t>(std::ceil(std::fmax(std::fmax(ul.y, ur.y), std::fmax(bl.y, br.y))));

        return Vec<int16_t, 4>(minX, minY, maxX - minX, maxY - minY);
    }
}

#include <corn/geometry/frame.h>

namespace corn {
    ReferenceFrame::ReferenceFrame(double left, double top, double right, double bottom)
        : left(left), top(top), right(right), bottom(bottom) {}

    Vec2 ReferenceFrame::map(const Vec4& relativeLocation) const {
        const Vec4& rl = relativeLocation;
        return {
            (1 - rl.z) * this->left + rl.z * this->right + rl.x,
            (1 - rl.w) * this->top + rl.w * this->bottom + rl.y
        };
    }
}

#include <corn/geometry/frame.h>

namespace corn {
    Vec2 ReferenceFrame::map(const Vec4& relativeLocation) const {
        const Vec4& rl = relativeLocation;
        return {
            (1 - rl.z) * this->left + rl.z * this->right + rl.x,
            (1 - rl.w) * this->top + rl.w * this->bottom + rl.y
        };
    }

    RelativeFrame::RelativeFrame(const Vec4& topLeft, const Vec4& bottomRight)
        : topLeft(topLeft), bottomRight(bottomRight) {}

    ReferenceFrame RelativeFrame::map(const ReferenceFrame& parentFrame) const {
        Vec2 tlAbsLoc = parentFrame.map(this->topLeft);
        Vec2 brAbsLoc = parentFrame.map(this->bottomRight);
        return {tlAbsLoc.x, tlAbsLoc.y, brAbsLoc.x, brAbsLoc.y};
    }
}

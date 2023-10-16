#pragma once

#include <corn/geometry/deg.h>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>

namespace corn {
    /**
     * @param v1 First vector.
     * @param v2 Second vector.
     * @return Euclidean distance between the two 2D vectors.
     */
    double dist(const Vec2& v1, const Vec2& v2);

    /**
     * @param v1 First vector.
     * @param v2 Second vector.
     * @return Euclidean distance between the two 3D vectors.
     */
    double dist(const Vec3& v1, const Vec3& v2);

    // TODO: vector rotation, etc.
}

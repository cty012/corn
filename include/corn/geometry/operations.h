#pragma once

#include <concepts>
#include <type_traits>
#include <corn/geometry/deg.h>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec3.h>

namespace corn {
    template <typename Vec>
    concept VectorType = std::is_same_v<Vec, Vec2> || std::is_same_v<Vec, Vec3>;

    /**
     * @param v1 First vector.
     * @param v2 Second vector.
     * @return Euclidean distance between the two vectors.
     */
    template <VectorType Vec>
    float dist(const Vec& v1, const Vec& v2);

    // TODO: vector rotation, etc.
}

#include "operations_template.cpp"

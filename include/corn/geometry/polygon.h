#pragma once

#include <corn/geometry/vec2.h>
#include <vector>

namespace corn {
    enum class PolygonType {
        EMPTY,             ///< The polygon has no vertices and holes.
        CONVEX,            ///< The polygon has no holes and is convex.
        CONCAVE,           ///< The polygon has no holes is concave.
        COMPLEX,           ///< The polygon has holes.
        INVALID            ///< The polygon has intersecting edges.
    };

    class Polygon {
    public:
        /**
         * @brief Create an empty polygon.
         */
        Polygon();

        /**
         * @brief Create a polygon from a list of vertices.
         * @param vertices The outer boundary of the polygon.
         */
        explicit Polygon(const std::vector<Vec2>& vertices);

        /**
         * @brief Create a polygon from a list of vertices and holes.
         * @param vertices The outer boundary of the polygon.
         * @param holes The holes of the polygon.
         */
        Polygon(const std::vector<Vec2>& vertices, const std::vector<std::vector<Vec2>>& holes);

        /**
         * @brief Get the outer boundary of the polygon.
         * @return The outer boundary of the polygon.
         */
        [[nodiscard]] const std::vector<Vec2>& getVertices() const;

        /**
         * @brief Get the holes of the polygon.
         * @return The holes of the polygon.
         */
        [[nodiscard]] const std::vector<std::vector<Vec2>>& getHoles() const;

        /**
         * @brief Set the vertices and holes of the polygon.
         * @param vertices The outer boundary of the polygon.
         * @param holes The holes of the polygon.
         */
        void setVertices(const std::vector<Vec2>& vertices, const std::vector<std::vector<Vec2>>& holes);

    private:
        /// @brief Triangulate the polygon.
        void triangulate();

        PolygonType type_;
        std::vector<Vec2> vertices_;
        std::vector<std::vector<Vec2>> holes_;
        std::vector<std::array<Vec2, 3>> triangles_;
    };
}

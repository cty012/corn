#pragma once

#include <corn/geometry/vec2.h>
#include <vector>

namespace corn {
    /**
     * @enum PolygonType
     * @brief Represents the type of a polygon.
     */
    enum class PolygonType {
        EMPTY,             ///< The polygon has no vertices and holes.
        CONVEX,            ///< The polygon has no holes and is convex.
        CONCAVE,           ///< The polygon has no holes and is concave.
        COMPLEX,           ///< The polygon has holes.
        INVALID            ///< The polygon has intersecting edges.
    };

    /**
     * @class Polygon
     * @brief Represents a single polygon with holes.
     *
     * The polygon is represented by a list of vertices and holes. The vertices represent the outer boundary of the
     * polygon, while the holes represent the inner boundaries.
     */
    class Polygon {
    public:
        /**
         * @brief Create an empty polygon.
         */
        Polygon() noexcept;

        /**
         * @brief Create a polygon from a list of vertices.
         * @param vertices The outer boundary of the polygon.
         */
        explicit Polygon(const std::vector<Vec2>& vertices) noexcept;

        /**
         * @brief Create a polygon from a list of vertices and holes.
         * @param vertices The outer boundary of the polygon.
         * @param holes The holes of the polygon.
         */
        Polygon(const std::vector<Vec2>& vertices, const std::vector<std::vector<Vec2>>& holes) noexcept;

        /**
         * @brief Create a polygon representing a triangle.
         * @param v1 The first vertex of the triangle.
         * @param v2 The second vertex of the triangle.
         * @param v3 The third vertex of the triangle.
         * @return The polygon representing the triangle.
         */
        static Polygon createTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3) noexcept;

        /**
         * @brief Create a polygon representing a rectangle.
         * @param topLeft The position of the rectangle's top-left corner.
         * @param width The width of the rectangle.
         * @param height The height of the rectangle.
         * @return The polygon representing the rectangle.
         */
        static Polygon createRectangle(const Vec2& topLeft, float width, float height) noexcept;

        /**
         * @brief Create a polygon representing a circle.
         * @param center The center of the circle.
         * @param radius The radius of the circle.
         * @param segments The number of segments to approximate the circle.
         * @return The polygon representing the circle.
         */
        static Polygon createCircle(const Vec2& center, float radius, size_t segments = 32) noexcept;

        /// @return The outer boundary of the polygon.
        [[nodiscard]] const std::vector<Vec2>& getVertices() const noexcept;

        /// @return The holes of the polygon.
        [[nodiscard]] const std::vector<std::vector<Vec2>>& getHoles() const noexcept;

        /**
         * @brief Set the vertices and holes of the polygon.
         * @param vertices The outer boundary of the polygon.
         * @param holes The holes of the polygon.
         */
        void setVertices(const std::vector<Vec2>& vertices, const std::vector<std::vector<Vec2>>& holes);

        /**
         * @param point The target point.
         * @param edgeInclusive Whether lying on the boundary qualifies as contained.
         * @return Whether the point is contained inside the polygon.
         */
        [[nodiscard]] bool contains(const Vec2& point, bool edgeInclusive) const;

        /**
         * @brief Translated the polygon by the displacement vector.
         * @param displacement The displacement vector.
         */
        void translate(const Vec2& displacement) noexcept;

        // Getters
        [[nodiscard]] PolygonType getType() const;
        [[nodiscard]] const Vec2& getCentroid() const;
        [[nodiscard]] float getArea() const;
        [[nodiscard]] const std::pair<Vec2, Vec2>& getBBox() const;
        [[nodiscard]] const std::vector<std::array<Vec2, 3>>& getTriangles() const;

    private:
        /// @brief Calculate the type of the polygon.
        void calcType() const;

        /// @brief Calculate the centroid and area of the polygon.
        void calcCentroidAndArea() const;

        /// @brief Calculate the bounding box of the polygon.
        void calcBBox() const;

        /// @brief Triangulate the polygon.
        void triangulate() const;

        mutable std::vector<Vec2> vertices_;
        mutable std::vector<std::vector<Vec2>> holes_;

        mutable PolygonType type_;
        mutable bool typeDirty_;

        mutable Vec2 centroid_;
        mutable float area_;
        mutable bool centroidAndAreaDirty_;

        mutable std::pair<Vec2, Vec2> bbox_;
        mutable bool bboxDirty_;

        mutable std::vector<std::array<Vec2, 3>> triangles_;
        mutable bool trianglesDirty_;
    };
}

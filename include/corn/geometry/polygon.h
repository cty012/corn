#pragma once

#include <corn/geometry/vec.h>
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
        explicit Polygon(const std::vector<Vec2f>& vertices) noexcept;

        /**
         * @brief Create a polygon from a list of vertices and holes.
         * @param vertices The outer boundary of the polygon.
         * @param holes The holes of the polygon.
         */
        Polygon(const std::vector<Vec2f>& vertices, const std::vector<std::vector<Vec2f>>& holes) noexcept;

        /**
         * @brief Create a polygon representing a triangle.
         * @param v1 The first vertex of the triangle.
         * @param v2 The second vertex of the triangle.
         * @param v3 The third vertex of the triangle.
         * @return The polygon representing the triangle.
         */
        static Polygon createTriangle(const Vec2f& v1, const Vec2f& v2, const Vec2f& v3) noexcept;

        /**
         * @brief Create a polygon representing a rectangle.
         * @param topLeft The position of the rectangle's top-left corner.
         * @param width The width of the rectangle.
         * @param height The height of the rectangle.
         * @return The polygon representing the rectangle.
         */
        static Polygon createRectangle(const Vec2f& topLeft, float width, float height) noexcept;

        /**
         * @brief Create a polygon representing a circle.
         * @param center The center of the circle.
         * @param radius The radius of the circle.
         * @param segments The number of segments to approximate the circle.
         * @return The polygon representing the circle.
         */
        static Polygon createCircle(const Vec2f& center, float radius, size_t segments = 32) noexcept;

        /// @return The outer boundary of the polygon.
        [[nodiscard]] const std::vector<Vec2f>& getVertices() const noexcept;

        /// @return The holes of the polygon.
        [[nodiscard]] const std::vector<std::vector<Vec2f>>& getHoles() const noexcept;

        /**
         * @brief Set the vertices and holes of the polygon.
         * @param vertices The outer boundary of the polygon.
         * @param holes The holes of the polygon.
         */
        void setVertices(const std::vector<Vec2f>& vertices, const std::vector<std::vector<Vec2f>>& holes);

        /**
         * @param point The target point.
         * @param edgeInclusive Whether lying on the boundary qualifies as contained.
         * @return Whether the point is contained inside the polygon.
         */
        [[nodiscard]] bool contains(const Vec2f& point, bool edgeInclusive) const;

        /**
         * @brief Translated the polygon by the displacement vector.
         * @param displacement The displacement vector.
         */
        void translate(const Vec2f& displacement) noexcept;

        // Getters
        [[nodiscard]] PolygonType getType() const;
        [[nodiscard]] const Vec2f& getCentroid() const;
        [[nodiscard]] float getArea() const;
        [[nodiscard]] const std::pair<Vec2f, Vec2f>& getBBox() const;
        [[nodiscard]] const std::vector<std::array<Vec2f, 3>>& getTriangles() const;

    private:
        /// @brief Calculate the type of the polygon.
        void calcType() const;

        /// @brief Calculate the centroid and area of the polygon.
        void calcCentroidAndArea() const;

        /// @brief Calculate the bounding box of the polygon.
        void calcBBox() const;

        /// @brief Triangulate the polygon.
        void triangulate() const;

        mutable std::vector<Vec2f> vertices_;
        mutable std::vector<std::vector<Vec2f>> holes_;

        mutable PolygonType type_;
        mutable bool typeDirty_;

        mutable Vec2f centroid_;
        mutable float area_;
        mutable bool centroidAndAreaDirty_;

        mutable std::pair<Vec2f, Vec2f> bbox_;
        mutable bool bboxDirty_;

        mutable std::vector<std::array<Vec2f, 3>> triangles_;
        mutable bool trianglesDirty_;
    };
}

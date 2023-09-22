#pragma once

namespace corn {
    /**
     * @class Vec2
     * @brief 2D vector class.
     */
    class Vec2 {
    public:
        double x;
        double y;

        // Ctors & dtors
        Vec2();
        Vec2(double x, double y);
        ~Vec2() = default;
        Vec2(const Vec2 &other);
        Vec2 &operator=(const Vec2 &other);

        // Operations
        /// @return A copy of the vector itself.
        Vec2 operator+() const;
        /// @return The reversed vector.
        Vec2 operator-() const;
        /// @return Result of adding this and other.
        Vec2 operator+(const Vec2 &other) const;
        /// @return Result of subtracting other from this.
        Vec2 operator-(const Vec2 &other) const;
        /// @return Add other to this vector.
        Vec2& operator+=(const Vec2 &other);
        /// @return Subtract other from this vector.
        Vec2& operator-=(const Vec2 &other);

        /// @return Dot product of this and other.
        [[nodiscard]] double dot(const Vec2 &other) const;
        /// @return Element-wise multiplication of this and other.
        Vec2 operator*(const Vec2 &other) const;
        /// @return Results of multiplying by a scalar.
        [[nodiscard]] Vec2 mult(double factor) const;

        /// @return 2-norm of the vector.
        [[nodiscard]] double norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec2 normalize() const;

        /**
         * @param v1 First vector.
         * @param v2 Second vector.
         * @return Euclidean distance between the two vectors.
         */
        friend double dist(const Vec2& v1, const Vec2& v2);
    };

    /**
     * @class Vec3
     * @brief 3D vector class.
     */
    class Vec3 {
    public:
        double x;
        double y;
        double z;

        // Ctors & dtors
        Vec3();
        Vec3(double x, double y, double z);
        ~Vec3() = default;
        Vec3(const Vec3 &other);
        Vec3 &operator=(const Vec3 &other);

        // Operations
        /// @return A copy of the vector itself.
        Vec3 operator+() const;
        /// @return The reversed vector.
        Vec3 operator-() const;
        /// @return Result of adding this and other.
        Vec3 operator+(const Vec3 &other) const;
        /// @return Result of subtracting other from this.
        Vec3 operator-(const Vec3 &other) const;
        /// @return Add other to this vector.
        Vec3& operator+=(const Vec3 &other);
        /// @return Subtract other from this vector.
        Vec3& operator-=(const Vec3 &other);

        /// @return Dot product of this and other.
        [[nodiscard]] double dot(const Vec3 &other) const;
        /// @return Element-wise multiplication of this and other.
        Vec3 operator*(const Vec3 &other) const;
        /// @return Results of multiplying by a scalar.
        [[nodiscard]] Vec3 mult(double factor) const;

        /// @return 2-norm of the vector.
        [[nodiscard]] double norm() const;
        /// @return Normalized vector. Zero if this is a zero vector.
        [[nodiscard]] Vec3 normalize() const;

        /**
         * @param v1 First vector.
         * @param v2 Second vector.
         * @return Euclidean distance between the two vectors.
         */
        friend double dist(const Vec3& v1, const Vec3& v2);
    };
}

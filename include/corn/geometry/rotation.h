#pragma once

#include <corn/geometry/deg.h>
#include <corn/geometry/vec3.h>
#include <corn/geometry/vec4.h>

namespace corn {
    struct Quaternion;

    /**
     * @class EulerAngles
     *
     * Yaw: rotation around the y axis (pointing down)
     * Pitch: rotation around the x axis (pointing right)
     * Roll: rotation around the z axis (pointing inside)
     *
     * Positive rotations are defined as counterclockwise.
     *
     * The Tait-Bryant convention is used to construct the final rotation. i.e. We apply the angles in the order of
     * roll, pitch, and yaw.
     *
     * @see Deg
     * @see Quaternion
     */
    struct EulerAngles {
        Deg yaw, pitch, roll;

        /// @brief Convert to quaternion.
        [[nodiscard]] Quaternion quat() const;
    };

    /**
     * @class Quaternion
     *
     * Quaternion is a better way to represent rotations than an Euler angle.
     *
     * The Quaternion class does not restrict the norm and the orientation of the quaternion in order to enforce unique
     * representation of rotations.
     *
     * @see Vec4
     * @see EulerAngles
     */
    struct Quaternion {
        Vec4 val;

        Quaternion(Vec4 val);  // NOLINT
        /// @brief Construct from the four components.
        Quaternion(float a, float b, float c, float d);
        /// @brief Construct from rotation around an axis.
        Quaternion(const Deg& theta, const Vec3& axis);
        // /// @brief Convert to Euler angles.
        // [[nodiscard]] EulerAngles euler() const;

        /// @brief Transform a 3D point.
        [[nodiscard]] Vec3 transform(const Vec3& point) const;

        // Operations
        /// @return A copy of the quaternion itself.
        Quaternion operator+() const;
        /// @return The reversed quaternion.
        Quaternion operator-() const;
        /// @return Result of adding this and other.
        Quaternion operator+(const Quaternion& other) const;
        /// @return Result of subtracting other from this.
        Quaternion operator-(const Quaternion& other) const;
        /**
         * @brief Add other to this quaternion in-place.
         * @return Reference to itself.
         */
        Quaternion& operator+=(const Quaternion& other);
        /**
         * @brief Subtract other from this quaternion in-place.
         * @return Reference to itself.
         */
        Quaternion& operator-=(const Quaternion& other);

        /// @return Multiplying this and other. Same as applying other and this on a 3D point.
        Quaternion operator*(const Quaternion& other) const;
        /// @return Multiplying this and other in-place.
        Quaternion& operator*=(const Quaternion& other);

        /// @return 2-norm of the quaternion.
        [[nodiscard]] float norm() const;
        /// @return Normalized quaternion. Zero if this is a zero quaternion.
        [[nodiscard]] Quaternion normalize() const;
        /// @return Inverse of the quaternion.
        [[nodiscard]] Quaternion inv() const;
    };
}

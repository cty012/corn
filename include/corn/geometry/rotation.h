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

        /// @brief Construct from Vec4.
        Quaternion(Vec4 val);  // NOLINT
        /// @brief Construct from the four components.
        Quaternion(float a, float b, float c, float d);
        /// @brief Construct from rotation around an axis.
        Quaternion(const Deg& theta, const Vec3& axis);

        /// @return 2-norm of the quaternion.
        [[nodiscard]] float norm() const;
        /// @return Normalized quaternion. Zero if this is a zero quaternion.
        [[nodiscard]] Quaternion normalize() const;
        /// @return Inverse of the quaternion.
        [[nodiscard]] Quaternion inv() const;
    };

    // Operations
    /// @return A copy of the quaternion itself.
    Quaternion operator+(const Quaternion& rhs);

    /// @return The reversed quaternion (represents the same rotation).
    Quaternion operator-(const Quaternion& rhs);

    /// @return Result of adding lhs and rhs.
    Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);

    /// @return Result of subtracting rhs from lhs.
    Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs);

    /// @return Multiplying lhs and rhs. Same as applying rhs and then lhs on a 3D point.
    Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

    /**
     * @brief Add rhs to lhs quaternion in-place.
     * @return Reference to lhs.
     */
    Quaternion& operator+=(Quaternion& lhs, const Quaternion& rhs);

    /**
     * @brief Subtract rhs from lhs quaternion in-place.
     * @return Reference to lhs.
     */
    Quaternion& operator-=(Quaternion& lhs, const Quaternion& rhs);

    /**
     * @brief In-place multiplication of lhs and rhs.
     * @return Reference to lhs.
     */
    Quaternion& operator*=(Quaternion& lhs, const Quaternion& rhs);
}

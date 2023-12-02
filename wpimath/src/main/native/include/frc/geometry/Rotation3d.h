// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>
#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "frc/geometry/Quaternion.h"
#include "frc/geometry/Rotation2d.h"
#include "units/angle.h"

namespace frc {

/**
 * A rotation in a 3D coordinate frame represented by a quaternion.
 */
class WPILIB_DLLEXPORT Rotation3d {
 public:
  /**
   * Constructs a Rotation3d with a default angle of 0 degrees.
   */
  Rotation3d() = default;

  /**
   * Constructs a Rotation3d from a quaternion.
   *
   * @param q The quaternion.
   */
  explicit Rotation3d(const Quaternion& q);

  /**
   * Constructs a Rotation3d from extrinsic roll, pitch, and yaw.
   *
   * Extrinsic rotations occur in that order around the axes in the fixed global
   * frame rather than the body frame.
   *
   * Angles are measured counterclockwise with the rotation axis pointing "out
   * of the page". If you point your right thumb along the positive axis
   * direction, your fingers curl in the direction of positive rotation.
   *
   * @param roll The counterclockwise rotation angle around the X axis (roll).
   * @param pitch The counterclockwise rotation angle around the Y axis (pitch).
   * @param yaw The counterclockwise rotation angle around the Z axis (yaw).
   */
  Rotation3d(units::radian_t roll, units::radian_t pitch, units::radian_t yaw);

  /**
   * Constructs a Rotation3d with the given axis-angle representation. The axis
   * doesn't have to be normalized.
   *
   * @param axis The rotation axis.
   * @param angle The rotation around the axis.
   */
  Rotation3d(const Eigen::Vector3d& axis, units::radian_t angle);

  /**
   * Constructs a Rotation3d with the given rotation vector representation. This
   * representation is equivalent to axis-angle, where the normalized axis is
   * multiplied by the rotation around the axis in radians.
   *
   * @param rvec The rotation vector.
   */
  explicit Rotation3d(const Eigen::Vector3d& rvec);

  /**
   * Constructs a Rotation3d from a rotation matrix.
   *
   * @param rotationMatrix The rotation matrix.
   * @throws std::domain_error if the rotation matrix isn't special orthogonal.
   */
  explicit Rotation3d(const Eigen::Matrix3d& rotationMatrix);

  /**
   * Constructs a Rotation3d that rotates the initial vector onto the final
   * vector.
   *
   * This is useful for turning a 3D vector (final) into an orientation relative
   * to a coordinate system vector (initial).
   *
   * @param initial The initial vector.
   * @param final The final vector.
   */
  Rotation3d(const Eigen::Vector3d& initial, const Eigen::Vector3d& final);

  /**
   * Adds two rotations together.
   *
   * @param other The rotation to add.
   *
   * @return The sum of the two rotations.
   */
  Rotation3d operator+(const Rotation3d& other) const;

  /**
   * Subtracts the new rotation from the current rotation and returns the new
   * rotation.
   *
   * @param other The rotation to subtract.
   *
   * @return The difference between the two rotations.
   */
  Rotation3d operator-(const Rotation3d& other) const;

  /**
   * Takes the inverse of the current rotation.
   *
   * @return The inverse of the current rotation.
   */
  Rotation3d operator-() const;

  /**
   * Multiplies the current rotation by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Rotation3d.
   */
  Rotation3d operator*(double scalar) const;

  /**
   * Divides the current rotation by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Rotation3d.
   */
  Rotation3d operator/(double scalar) const;

  /**
   * Checks equality between this Rotation3d and another object.
   */
  bool operator==(const Rotation3d&) const;

  /**
   * Adds the new rotation to the current rotation. The other rotation is
   * applied extrinsically, which means that it rotates around the global axes.
   * For example, Rotation3d{90_deg, 0, 0}.RotateBy(Rotation3d{0, 45_deg, 0})
   * rotates by 90 degrees around the +X axis and then by 45 degrees around the
   * global +Y axis. (This is equivalent to Rotation3d{90_deg, 45_deg, 0})
   *
   * @param other The extrinsic rotation to rotate by.
   *
   * @return The new rotated Rotation3d.
   */
  Rotation3d RotateBy(const Rotation3d& other) const;

  /**
   * Returns the quaternion representation of the Rotation3d.
   */
  const Quaternion& GetQuaternion() const;

  /**
   * Returns the counterclockwise rotation angle around the X axis (roll).
   */
  units::radian_t X() const;

  /**
   * Returns the counterclockwise rotation angle around the Y axis (pitch).
   */
  units::radian_t Y() const;

  /**
   * Returns the counterclockwise rotation angle around the Z axis (yaw).
   */
  units::radian_t Z() const;

  /**
   * Returns the axis in the axis-angle representation of this rotation.
   */
  Eigen::Vector3d Axis() const;

  /**
   * Returns the angle in the axis-angle representation of this rotation.
   */
  units::radian_t Angle() const;

  /**
   * Returns a Rotation2d representing this Rotation3d projected into the X-Y
   * plane.
   */
  Rotation2d ToRotation2d() const;

 private:
  Quaternion m_q;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Rotation3d& rotation);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Rotation3d& rotation);

}  // namespace frc

#include "frc/geometry/proto/Rotation3dProto.h"
#include "frc/geometry/struct/Rotation3dStruct.h"

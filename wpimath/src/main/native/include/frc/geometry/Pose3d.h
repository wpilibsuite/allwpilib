// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation3d.h"
#include "frc/geometry/Transform3d.h"
#include "frc/geometry/Translation3d.h"
#include "frc/geometry/Twist3d.h"

namespace frc {

/**
 * Represents a 3D pose containing translational and rotational elements.
 */
class WPILIB_DLLEXPORT Pose3d {
 public:
  /**
   * Constructs a pose at the origin facing toward the positive X axis.
   */
  constexpr Pose3d() = default;

  /**
   * Constructs a pose with the specified translation and rotation.
   *
   * @param translation The translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  Pose3d(Translation3d translation, Rotation3d rotation);

  /**
   * Constructs a pose with x, y, and z translations instead of a separate
   * Translation3d.
   *
   * @param x The x component of the translational component of the pose.
   * @param y The y component of the translational component of the pose.
   * @param z The z component of the translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  Pose3d(units::meter_t x, units::meter_t y, units::meter_t z,
         Rotation3d rotation);

  /**
   * Constructs a 3D pose from a 2D pose in the X-Y plane.
   *
   * @param pose The 2D pose.
   */
  explicit Pose3d(const Pose2d& pose);

  /**
   * Transforms the pose by the given transformation and returns the new
   * transformed pose. The transform is applied relative to the pose's frame.
   * Note that this differs from Pose3d::RotateBy(const Rotation3d&), which is
   * applied relative to the global frame and around the origin.
   *
   * @param other The transform to transform the pose by.
   *
   * @return The transformed pose.
   */
  Pose3d operator+(const Transform3d& other) const;

  /**
   * Returns the Transform3d that maps the one pose to another.
   *
   * @param other The initial pose of the transformation.
   * @return The transform that maps the other pose to the current pose.
   */
  Transform3d operator-(const Pose3d& other) const;

  /**
   * Checks equality between this Pose3d and another object.
   */
  bool operator==(const Pose3d&) const = default;

  /**
   * Returns the underlying translation.
   *
   * @return Reference to the translational component of the pose.
   */
  const Translation3d& Translation() const { return m_translation; }

  /**
   * Returns the X component of the pose's translation.
   *
   * @return The x component of the pose's translation.
   */
  units::meter_t X() const { return m_translation.X(); }

  /**
   * Returns the Y component of the pose's translation.
   *
   * @return The y component of the pose's translation.
   */
  units::meter_t Y() const { return m_translation.Y(); }

  /**
   * Returns the Z component of the pose's translation.
   *
   * @return The z component of the pose's translation.
   */
  units::meter_t Z() const { return m_translation.Z(); }

  /**
   * Returns the underlying rotation.
   *
   * @return Reference to the rotational component of the pose.
   */
  const Rotation3d& Rotation() const { return m_rotation; }

  /**
   * Multiplies the current pose by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Pose2d.
   */
  Pose3d operator*(double scalar) const;

  /**
   * Divides the current pose by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Pose2d.
   */
  Pose3d operator/(double scalar) const;

  /**
   * Rotates the pose around the origin and returns the new pose.
   *
   * @param other The rotation to transform the pose by, which is applied
   * extrinsically (from the global frame).
   *
   * @return The rotated pose.
   */
  Pose3d RotateBy(const Rotation3d& other) const;

  /**
   * Transforms the pose by the given transformation and returns the new
   * transformed pose. The transform is applied relative to the pose's frame.
   * Note that this differs from Pose3d::RotateBy(const Rotation3d&), which is
   * applied relative to the global frame and around the origin.
   *
   * @param other The transform to transform the pose by.
   *
   * @return The transformed pose.
   */
  Pose3d TransformBy(const Transform3d& other) const;

  /**
   * Returns the current pose relative to the given pose.
   *
   * This function can often be used for trajectory tracking or pose
   * stabilization algorithms to get the error between the reference and the
   * current pose.
   *
   * @param other The pose that is the origin of the new coordinate frame that
   * the current pose will be converted into.
   *
   * @return The current pose relative to the new origin pose.
   */
  Pose3d RelativeTo(const Pose3d& other) const;

  /**
   * Obtain a new Pose3d from a (constant curvature) velocity.
   *
   * The twist is a change in pose in the robot's coordinate frame since the
   * previous pose update. When the user runs exp() on the previous known
   * field-relative pose with the argument being the twist, the user will
   * receive the new field-relative pose.
   *
   * "Exp" represents the pose exponential, which is solving a differential
   * equation moving the pose forward in time.
   *
   * @param twist The change in pose in the robot's coordinate frame since the
   * previous pose update. For example, if a non-holonomic robot moves forward
   * 0.01 meters and changes angle by 0.5 degrees since the previous pose
   * update, the twist would be Twist3d{0.01_m, 0_m, 0_m, Rotation3d{0.0, 0.0,
   * 0.5_deg}}.
   *
   * @return The new pose of the robot.
   */
  Pose3d Exp(const Twist3d& twist) const;

  /**
   * Returns a Twist3d that maps this pose to the end pose. If c is the output
   * of a.Log(b), then a.Exp(c) would yield b.
   *
   * @param end The end pose for the transformation.
   *
   * @return The twist that maps this to end.
   */
  Twist3d Log(const Pose3d& end) const;

  /**
   * Returns a Pose2d representing this Pose3d projected into the X-Y plane.
   */
  Pose2d ToPose2d() const;

 private:
  Translation3d m_translation;
  Rotation3d m_rotation;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Pose3d& pose);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Pose3d& pose);

}  // namespace frc

#include "frc/geometry/proto/Pose3dProto.h"
#include "frc/geometry/struct/Pose3dStruct.h"

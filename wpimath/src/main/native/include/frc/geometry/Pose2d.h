// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>

#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Transform2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/geometry/Twist2d.h"

namespace frc {

/**
 * Represents a 2D pose containing translational and rotational elements.
 */
class WPILIB_DLLEXPORT Pose2d {
 public:
  /**
   * Constructs a pose at the origin facing toward the positive X axis.
   */
  constexpr Pose2d() = default;

  /**
   * Constructs a pose with the specified translation and rotation.
   *
   * @param translation The translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  constexpr Pose2d(Translation2d translation, Rotation2d rotation);

  /**
   * Constructs a pose with x and y translations instead of a separate
   * Translation2d.
   *
   * @param x The x component of the translational component of the pose.
   * @param y The y component of the translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  constexpr Pose2d(units::meter_t x, units::meter_t y, Rotation2d rotation);

  /**
   * Transforms the pose by the given transformation and returns the new
   * transformed pose.
   *
   * <pre>
   * [x_new]    [cos, -sin, 0][transform.x]
   * [y_new] += [sin,  cos, 0][transform.y]
   * [t_new]    [  0,    0, 1][transform.t]
   * </pre>
   *
   * @param other The transform to transform the pose by.
   *
   * @return The transformed pose.
   */
  constexpr Pose2d operator+(const Transform2d& other) const;

  /**
   * Returns the Transform2d that maps the one pose to another.
   *
   * @param other The initial pose of the transformation.
   * @return The transform that maps the other pose to the current pose.
   */
  Transform2d operator-(const Pose2d& other) const;

  /**
   * Checks equality between this Pose2d and another object.
   */
  bool operator==(const Pose2d&) const = default;

  /**
   * Returns the underlying translation.
   *
   * @return Reference to the translational component of the pose.
   */
  constexpr const Translation2d& Translation() const { return m_translation; }

  /**
   * Returns the X component of the pose's translation.
   *
   * @return The x component of the pose's translation.
   */
  constexpr units::meter_t X() const { return m_translation.X(); }

  /**
   * Returns the Y component of the pose's translation.
   *
   * @return The y component of the pose's translation.
   */
  constexpr units::meter_t Y() const { return m_translation.Y(); }

  /**
   * Returns the underlying rotation.
   *
   * @return Reference to the rotational component of the pose.
   */
  constexpr const Rotation2d& Rotation() const { return m_rotation; }

  /**
   * Multiplies the current pose by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Pose2d.
   */
  constexpr Pose2d operator*(double scalar) const;

  /**
   * Divides the current pose by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Pose2d.
   */
  constexpr Pose2d operator/(double scalar) const;

  /**
   * Rotates the pose around the origin and returns the new pose.
   *
   * @param other The rotation to transform the pose by.
   *
   * @return The rotated pose.
   */
  constexpr Pose2d RotateBy(const Rotation2d& other) const;

  /**
   * Transforms the pose by the given transformation and returns the new pose.
   * See + operator for the matrix multiplication performed.
   *
   * @param other The transform to transform the pose by.
   *
   * @return The transformed pose.
   */
  constexpr Pose2d TransformBy(const Transform2d& other) const;

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
  Pose2d RelativeTo(const Pose2d& other) const;

  /**
   * Obtain a new Pose2d from a (constant curvature) velocity.
   *
   * See https://file.tavsys.net/control/controls-engineering-in-frc.pdf section
   * 10.2 "Pose exponential" for a derivation.
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
   * update, the twist would be Twist2d{0.01_m, 0_m, 0.5_deg}.
   *
   * @return The new pose of the robot.
   */
  Pose2d Exp(const Twist2d& twist) const;

  /**
   * Returns a Twist2d that maps this pose to the end pose. If c is the output
   * of a.Log(b), then a.Exp(c) would yield b.
   *
   * @param end The end pose for the transformation.
   *
   * @return The twist that maps this to end.
   */
  Twist2d Log(const Pose2d& end) const;

  /**
   * Returns the nearest Pose2d from a collection of poses
   * @param poses The collection of poses.
   * @return The nearest Pose2d from the collection.
   */
  Pose2d Nearest(std::span<const Pose2d> poses) const;

  /**
   * Returns the nearest Pose2d from a collection of poses
   * @param poses The collection of poses.
   * @return The nearest Pose2d from the collection.
   */
  Pose2d Nearest(std::initializer_list<Pose2d> poses) const;

 private:
  Translation2d m_translation;
  Rotation2d m_rotation;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Pose2d& pose);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Pose2d& pose);

}  // namespace frc

#include "frc/geometry/proto/Pose2dProto.h"
#include "frc/geometry/struct/Pose2dStruct.h"
#include "frc/geometry/Pose2d.inc"

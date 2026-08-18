// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/trajectory/HolonomicSample.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/curvature.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpi::math {

/**
 * Represents a single sample in a spline-based drivetrain trajectory.
 */
class DrivetrainSplineSample : public HolonomicSample {
 public:
  /** The curvature of the path at this sample. */
  wpi::units::curvature_t curvature{0.0};

  /** Constructs a default DrivetrainSplineSample with all zero values. */
  constexpr DrivetrainSplineSample() = default;

  /**
   * Constructs a SplineSample from path-relative scalars.
   *
   * The robot follows the path along its heading, so the scalar forward
   * velocity and acceleration are rotated into the field frame using the
   * sample's pose. The resulting velocity and acceleration are field-relative.
   *
   * @param time The time of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The signed forward (path-tangential) velocity.
   * @param acceleration The signed forward (path-tangential) acceleration.
   * @param curvature The curvature of the path at this sample.
   */
  constexpr DrivetrainSplineSample(
      wpi::units::second_t time, const Pose2d& pose,
      wpi::units::meters_per_second_t velocity,
      wpi::units::meters_per_second_squared_t acceleration,
      wpi::units::curvature_t curvature)
      : HolonomicSample{time, pose,
                        ChassisVelocities{velocity, 0_mps, velocity * curvature}
                            .ToFieldRelative(pose.Rotation()),
                        ChassisAccelerations{acceleration, 0_mps_sq,
                                             acceleration * curvature}
                            .ToFieldRelative(pose.Rotation())},
        curvature{curvature} {}

  /**
   * Constructs a SplineSample.
   *
   * @param time The time of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the field reference
   *     frame).
   * @param acceleration The robot acceleration at this sample (in the field
   *     reference frame).
   * @param curvature The curvature of the path at this sample.
   */
  constexpr DrivetrainSplineSample(wpi::units::second_t time,
                                   const Pose2d& pose,
                                   const ChassisVelocities& velocity,
                                   const ChassisAccelerations& acceleration,
                                   wpi::units::curvature_t curvature)
      : HolonomicSample{time, pose, velocity, acceleration},
        curvature{curvature} {}

  /**
   * Constructs a SplineSample from a generic sample.
   * Curvature is calculated as omega / forward velocity.
   *
   * @param sample The HolonomicSample to convert.
   */
  explicit constexpr DrivetrainSplineSample(const HolonomicSample& sample)
      : HolonomicSample{sample.time, sample.pose, sample.velocity,
                        sample.acceleration} {
    auto vx = sample.velocity.ToRobotRelative(sample.pose.Rotation()).vx;
    curvature = sample.velocity.omega / (vx == 0_mps ? 1e-9_mps : vx);
  }

  /**
   * Returns the signed forward (robot-relative tangential) velocity at this
   * sample. This is the field-relative velocity projected onto the sample's
   * heading.
   *
   * @return The forward velocity.
   */
  constexpr wpi::units::meters_per_second_t ForwardVelocity() const {
    return velocity.ToRobotRelative(pose.Rotation()).vx;
  }

  /**
   * Returns the signed forward (robot-relative tangential) acceleration at this
   * sample. This is the field-relative acceleration projected onto the sample's
   * heading.
   *
   * @return The forward acceleration.
   */
  constexpr wpi::units::meters_per_second_squared_t ForwardAcceleration()
      const {
    return acceleration.ToRobotRelative(pose.Rotation()).ax;
  }

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  constexpr DrivetrainSplineSample Transform(
      const Transform2d& transform) const {
    return DrivetrainSplineSample{
        time, pose.TransformBy(transform),
        velocity.ToFieldRelative(transform.Rotation()),
        acceleration.ToFieldRelative(transform.Rotation()), curvature};
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  constexpr DrivetrainSplineSample RelativeTo(const Pose2d& other) const {
    return DrivetrainSplineSample{
        time, pose.RelativeTo(other),
        velocity.ToRobotRelative(other.Rotation()),
        acceleration.ToRobotRelative(other.Rotation()), curvature};
  }

  /**
   * Checks equality between this SplineSample and another object.
   *
   * @return True if the samples are equal.
   */
  constexpr bool operator==(const DrivetrainSplineSample& other) const =
      default;
};

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const DrivetrainSplineSample& sample);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, DrivetrainSplineSample& sample);

}  // namespace wpi::math

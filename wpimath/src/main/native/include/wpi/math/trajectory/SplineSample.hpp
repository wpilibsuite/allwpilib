// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/trajectory/TrajectorySample.hpp"
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
 * Represents a single sample in a spline-based trajectory.
 */
class SplineSample {
 public:
  /** The timestamp of the sample relative to the trajectory start. */
  wpi::units::second_t timestamp{0.0};
  /** The robot pose at this sample (in the field reference frame). */
  Pose2d pose;
  /** The robot velocity at this sample (in the field reference frame). */
  ChassisVelocities velocity;
  /** The robot acceleration at this sample (in the field reference frame). */
  ChassisAccelerations acceleration;
  /** The curvature of the path at this sample. */
  wpi::units::curvature_t curvature{0.0};

  /** Constructs a default SplineSample with all zero values. */
  constexpr SplineSample() = default;

  /**
   * Constructs a SplineSample from path-relative scalars.
   *
   * The robot follows the path along its heading, so the scalar forward
   * velocity and acceleration are rotated into the field frame using the
   * sample's pose. The resulting velocity and acceleration are field-relative.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory
   *     start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The signed forward (path-tangential) velocity.
   * @param acceleration The signed forward (path-tangential) acceleration.
   * @param curvature The curvature of the path at this sample.
   */
  constexpr SplineSample(wpi::units::second_t timestamp, const Pose2d& pose,
                         wpi::units::meters_per_second_t velocity,
                         wpi::units::meters_per_second_squared_t acceleration,
                         wpi::units::curvature_t curvature)
      : timestamp{timestamp},
        pose{pose},
        velocity{ChassisVelocities{velocity, 0_mps, velocity * curvature}
                     .ToFieldRelative(pose.Rotation())},
        acceleration{ChassisAccelerations{acceleration, 0_mps_sq,
                                          acceleration * curvature}
                         .ToFieldRelative(pose.Rotation())},
        curvature{curvature} {}

  /**
   * Constructs a SplineSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory
   * start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the field reference
   *     frame).
   * @param acceleration The robot acceleration at this sample (in the field
   *     reference frame).
   * @param curvature The curvature of the path at this sample.
   */
  constexpr SplineSample(wpi::units::second_t timestamp, const Pose2d& pose,
                         const ChassisVelocities& velocity,
                         const ChassisAccelerations& acceleration,
                         wpi::units::curvature_t curvature)
      : timestamp{timestamp},
        pose{pose},
        velocity{velocity},
        acceleration{acceleration},
        curvature{curvature} {}

  /**
   * Constructs a SplineSample from a generic sample.
   * Curvature is calculated as omega / forward velocity.
   *
   * @param sample The TrajectorySample to convert.
   */
  explicit constexpr SplineSample(const TrajectorySample& sample)
      : timestamp{sample.timestamp},
        pose{sample.pose},
        velocity{sample.velocity},
        acceleration{sample.acceleration} {
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
  constexpr SplineSample Transform(const Transform2d& transform) const {
    return SplineSample{timestamp, pose.TransformBy(transform),
                        velocity.ToFieldRelative(transform.Rotation()),
                        acceleration.ToFieldRelative(transform.Rotation()),
                        curvature};
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  constexpr SplineSample RelativeTo(const Pose2d& other) const {
    return SplineSample{timestamp, pose.RelativeTo(other),
                        velocity.ToRobotRelative(other.Rotation()),
                        acceleration.ToRobotRelative(other.Rotation()),
                        curvature};
  }

  /**
   * Creates a new sample with the given timestamp.
   *
   * @param newTimestamp The new timestamp.
   * @return A new sample with the given timestamp.
   */
  constexpr SplineSample WithNewTimestamp(
      wpi::units::second_t newTimestamp) const {
    return SplineSample{newTimestamp, pose, velocity, acceleration, curvature};
  }

  /**
   * Checks equality between this SplineSample and another object.
   *
   * @return True if the samples are equal.
   */
  constexpr bool operator==(const SplineSample& other) const = default;
};

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const SplineSample& sample);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, SplineSample& sample);

}  // namespace wpi::math

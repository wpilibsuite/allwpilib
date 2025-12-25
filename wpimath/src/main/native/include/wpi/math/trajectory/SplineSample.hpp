// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/units/curvature.hpp"
#include "wpi/units/time.hpp"

namespace wpi::math {

/**
 * Represents a single sample in a spline-based trajectory.
 */
class SplineSample : public TrajectorySample<SplineSample> {
 public:
  /**
   * The curvature of the path at this sample, in 1/m.
   */
  wpi::units::curvature_t curvature{0.0};

  constexpr SplineSample() = default;

  /**
   * Constructs a SplineSample.
   *
   * @param time The timestamp of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference
   *            frame).
   * @param acc The robot acceleration at this sample (in the robot's reference
   *            frame).
   * @param curvature The curvature of the path at this sample, in 1/m.
   */
  constexpr SplineSample(wpi::units::second_t time, const Pose2d& pose,
                         const ChassisSpeeds& vel,
                         const ChassisAccelerations& acc,
                         wpi::units::curvature_t curvature)
      : TrajectorySample(time, pose, vel, acc), curvature(curvature) {}

  /**
   * Constructs a SplineSample from seconds as double for convenience.
   *
   * @param timeSeconds The timestamp in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param linearVelocity The linear velocity in m/s.
   * @param linearAcceleration The linear acceleration in m/s².
   * @param curvature The curvature of the path at this sample, in 1/m.
   */
  explicit constexpr SplineSample(double timeSeconds, const Pose2d& pose,
                                  double linearVelocity,
                                  double linearAcceleration, double curvature)
      : TrajectorySample(
            wpi::units::second_t{timeSeconds}, pose,
            ChassisSpeeds{
                wpi::units::meters_per_second_t{linearVelocity}, 0_mps,
                wpi::units::radians_per_second_t{linearVelocity * curvature}},
            ChassisAccelerations{
                wpi::units::meters_per_second_squared_t{linearAcceleration},
                0_mps_sq,
                wpi::units::radians_per_second_squared_t{linearAcceleration *
                                                         curvature}}),
        curvature(wpi::units::curvature_t{curvature}) {}

  /**
   * Constructs a SplineSample by converting from a generic sample.
   * Curvature is calculated as omega / vx.
   *
   * @param sample The TrajectorySample to convert.
   */
  template <typename T>
  explicit constexpr SplineSample(const TrajectorySample<T>& sample)
      : TrajectorySample(sample.timestamp, sample.pose, sample.velocity,
                         sample.acceleration),
        curvature(
            wpi::units::curvature_t{sample.velocity.vx.value() == 0.0
                                        ? (sample.velocity.omega.value() / 1e-9)
                                        : (sample.velocity.omega.value() /
                                           sample.velocity.vx.value())}) {}

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  constexpr SplineSample Transform(const Transform2d& transform) const {
    return SplineSample{timestamp, pose.TransformBy(transform), velocity,
                        acceleration, curvature};
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  constexpr SplineSample RelativeTo(const Pose2d& other) const {
    return SplineSample{timestamp, pose.RelativeTo(other), velocity,
                        acceleration, curvature};
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
   */
  constexpr bool operator==(const SplineSample& other) const = default;
};

}  // namespace wpi::math

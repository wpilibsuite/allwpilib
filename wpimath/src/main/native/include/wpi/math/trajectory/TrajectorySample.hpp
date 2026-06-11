// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include "Trajectory.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpi::math {
/**
 * Represents a single sample in a trajectory.
 */
class TrajectorySample {
 public:
  /** The timestamp of the sample relative to the trajectory start. */
  wpi::units::second_t timestamp{0.0};
  /** The robot pose at this sample (in the field reference frame). */
  Pose2d pose;
  /** The robot velocity at this sample (in the field reference frame). */
  ChassisVelocities velocity;
  /** The robot acceleration at this sample (in the field reference frame). */
  ChassisAccelerations acceleration;

  /** Constructs a default TrajectorySample with all zero values. */
  constexpr TrajectorySample() = default;

  /**
   * Constructs a TrajectorySample.
   *
   * @param time The timestamp of the sample relative to the trajectory start.
   * @param p The robot pose at this sample (in the field reference frame).
   * @param v The robot velocity at this sample (in the field reference frame).
   * @param a The robot acceleration at this sample (in the field reference
   *          frame).
   */
  constexpr TrajectorySample(wpi::units::second_t time, const Pose2d& p,
                             const ChassisVelocities& v,
                             const ChassisAccelerations& a)
      : timestamp(time), pose(p), velocity(v), acceleration(a) {}

  /**
   * Checks equality between this TrajectorySample and another.
   *
   * @return True if the samples are equal.
   */
  constexpr bool operator==(const TrajectorySample& other) const = default;

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  constexpr TrajectorySample Transform(const Transform2d& transform) const {
    return {timestamp, pose.TransformBy(transform),
            velocity.ToFieldRelative(transform.Rotation()),
            acceleration.ToFieldRelative(transform.Rotation())};
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  constexpr TrajectorySample RelativeTo(const Pose2d& other) const {
    return {timestamp, pose.RelativeTo(other),
            velocity.ToRobotRelative(other.Rotation()),
            acceleration.ToRobotRelative(other.Rotation())};
  }

  /**
   * Creates a new sample with the given timestamp.
   *
   * @param newTimestamp The new timestamp.
   * @return A new sample with the given timestamp.
   */
  constexpr TrajectorySample WithNewTimestamp(
      wpi::units::second_t newTimestamp) const {
    return {newTimestamp, pose, velocity, acceleration};
  }

  ~TrajectorySample() = default;
};

/**
 * Static kinematic interpolation between two samples (constant accel).
 *
 * @param start The start sample.
 * @param end The end sample.
 * @param t The interpolation parameter [0, 1].
 * @return A new interpolated base sample.
 */
constexpr TrajectorySample KinematicInterpolate(const TrajectorySample& start,
                                                const TrajectorySample& end,
                                                double t) {
  if (t <= 0.0) {
    return {start.timestamp, start.pose, start.velocity, start.acceleration};
  } else if (t >= 1.0) {
    return {end.timestamp, end.pose, end.velocity, end.acceleration};
  }

  // Absolute timestamp of the interpolated sample
  const auto interpTime = wpi::util::Lerp(start.timestamp, end.timestamp, t);

  // Elapsed time from the start sample, used for integration
  const auto deltaT = interpTime - start.timestamp;

  // Interpolate acceleration
  ChassisAccelerations newAccel{
      wpi::util::Lerp(start.acceleration.ax, end.acceleration.ax, t),
      wpi::util::Lerp(start.acceleration.ay, end.acceleration.ay, t),
      wpi::util::Lerp(start.acceleration.alpha, end.acceleration.alpha, t)};

  // vₖ₊₁ = vₖ + aₖΔt
  ChassisVelocities newVel{
      start.velocity.vx + start.acceleration.ax * deltaT,
      start.velocity.vy + start.acceleration.ay * deltaT,
      start.velocity.omega + start.acceleration.alpha * deltaT};

  // xₖ₊₁ = xₖ + vₖΔt + ½aₖ(Δt)²
  Pose2d newPose{start.pose.Translation().X() + start.velocity.vx * deltaT +
                     0.5 * start.acceleration.ax * deltaT * deltaT,
                 start.pose.Translation().Y() + start.velocity.vy * deltaT +
                     0.5 * start.acceleration.ay * deltaT * deltaT,
                 Rotation2d{start.pose.Rotation().Radians() +
                            start.velocity.omega * deltaT +
                            0.5 * start.acceleration.alpha * deltaT * deltaT}};

  return TrajectorySample{interpTime, newPose, newVel, newAccel};
}

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const TrajectorySample& sample);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, TrajectorySample& sample);

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json,
             const std::vector<TrajectorySample>& samples);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json,
               std::vector<TrajectorySample>& samples);

}  // namespace wpi::math

#include "wpi/math/trajectory/proto/TrajectorySampleProto.hpp"
#include "wpi/math/trajectory/struct/TrajectorySampleStruct.hpp"

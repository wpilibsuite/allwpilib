// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/MathExtras.hpp"

namespace wpi::math {

/**
 * Represents a single sample in a trajectory.
 *
 * @tparam SampleType The sample class type (e.g., DifferentialSample)
 */
template <typename SampleType>
  requires std::derived_from<SampleType, TrajectorySample>
class TrajectorySample {
 public:
  using seconds_t = wpi::units::second_t;

  seconds_t timestamp;                // time since trajectory start
  Pose2d pose;                        // field-relative pose
  ChassisSpeeds velocity;             // robot-relative velocity
  ChassisAccelerations acceleration;  // robot-relative acceleration

  constexpr TrajectorySample(const seconds_t time, const Pose2d& p,
                             const ChassisSpeeds& v,
                             const ChassisAccelerations& a)
      : timestamp(time), pose(p), velocity(v), acceleration(a) {}

  // Equality
  constexpr bool operator==(const TrajectorySample& other) const = default;

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  constexpr SampleType Transform(const Transform2d& transform) const {
    return static_cast<const SampleType*>(this)->Transform(transform);
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  constexpr SampleType RelativeTo(const Pose2d& other) const {
    return static_cast<const SampleType*>(this)->RelativeTo(other);
  }

  /**
   * Creates a new sample with the given timestamp.
   *
   * @param timestamp The new timestamp.
   * @return A new sample with the given timestamp.
   */
  constexpr SampleType WithNewTimestamp(seconds_t newTimestamp) const {
    return static_cast<const SampleType*>(this)->WithNewTimestamp(newTimestamp);
  }
};

// Forward declaration for Base
class TrajectorySampleBase;

/**
 * A base concrete sample type with no additional fields.
 */
class TrajectorySampleBase : public TrajectorySample<TrajectorySampleBase> {
 public:
  constexpr TrajectorySampleBase(const seconds_t time, const Pose2d& p,
                                 const ChassisSpeeds& v,
                                 const ChassisAccelerations& a)
      : TrajectorySample{time, p, v, a} {}

  // Copy from any TrajectorySample
  template <typename T>
  explicit constexpr TrajectorySampleBase(const TrajectorySample<T>& s)
      : TrajectorySample{s.timestamp, s.pose, s.velocity, s.acceleration} {}

  constexpr TrajectorySampleBase Transform(const Transform2d& transform) const {
    return TrajectorySampleBase{timestamp, pose.TransformBy(transform),
                                velocity, acceleration};
  }

  constexpr TrajectorySampleBase RelativeTo(const Pose2d& other) const {
    return TrajectorySampleBase{timestamp, pose.RelativeTo(other), velocity,
                                acceleration};
  }

  constexpr TrajectorySampleBase WithNewTimestamp(
      seconds_t newTimestamp) const {
    return TrajectorySampleBase{newTimestamp, pose, velocity, acceleration};
  }
};

/**
 * Static kinematic interpolation between two samples (constant accel).
 *
 * @param start The start sample.
 * @param end The end sample.
 * @param t The interpolation parameter [0, 1].
 * @return A new interpolated base sample.
 */
template <typename T>
TrajectorySampleBase KinematicInterpolate(const TrajectorySample<T>& start,
                                          const TrajectorySample<T>& end,
                                          double t) {
  if (t <= 0.0) {
    return TrajectorySampleBase{start.timestamp, start.pose, start.velocity,
                                start.acceleration};
  } else if (t >= 1.0) {
    return TrajectorySampleBase{end.timestamp, end.pose, end.velocity,
                                end.acceleration};
  }

  // Interpolated delta time between start and end timestamps
  const auto interpT = wpi::util::Lerp(start.timestamp, end.timestamp, t);

  // Interpolate acceleration
  ChassisAccelerations newAccel{
      wpi::util::Lerp(start.acceleration.ax, end.acceleration.ax, t),
      wpi::util::Lerp(start.acceleration.ay, end.acceleration.ay, t),
      wpi::util::Lerp(start.acceleration.alpha, end.acceleration.alpha, t)};

  // v_{k+1} = v_k + a_k * dt
  ChassisSpeeds newVel{
      start.velocity.vx + start.acceleration.ax * interpT,
      start.velocity.vy + start.acceleration.ay * interpT,
      start.velocity.omega + start.acceleration.alpha * interpT};

  // x_{k+1} = x_k + v_k * dt + 0.5 a (dt)^2
  Pose2d newPose{
      start.pose.Translation().X() + start.velocity.vx * interpT +
          0.5 * start.acceleration.ax * interpT * interpT,
      start.pose.Translation().Y() + start.velocity.vy * interpT +
          0.5 * start.acceleration.ay * interpT * interpT,
      Rotation2d{start.pose.Rotation().Radians() +
                 start.velocity.omega * interpT +
                 0.5 * start.acceleration.alpha * interpT * interpT}};

  return TrajectorySampleBase{interpT, newPose, newVel, newAccel};
}

}  // namespace wpi::math

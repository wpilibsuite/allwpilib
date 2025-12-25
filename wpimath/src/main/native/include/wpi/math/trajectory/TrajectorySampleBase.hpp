// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/MathExtras.hpp"

namespace wpi::math {

// Forward declaration for Base
class TrajectorySampleBase;

/**
 * A base concrete sample type with no additional fields.
 */
class TrajectorySampleBase : public TrajectorySample<TrajectorySampleBase> {
 public:
  constexpr TrajectorySampleBase(wpi::units::second_t time, const Pose2d& p,
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
      wpi::units::second_t newTimestamp) const {
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

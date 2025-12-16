// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <memory>

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
 */
class TrajectorySample {
 public:
  using seconds_t = wpi::units::second_t;

  const seconds_t timestamp;                      // time since trajectory start
  const Pose2d pose;                                     // field-relative pose
  const ChassisSpeeds velocity;                          // robot-relative velocity
  const ChassisAccelerations acceleration;

  constexpr TrajectorySample(const seconds_t time,
                             const Pose2d& p,
                             const ChassisSpeeds& v,
                             const ChassisAccelerations& a)
      : timestamp(time), pose(p), velocity(v), acceleration(a) {}

  // Equality
  constexpr bool operator==(const TrajectorySample& other) const = default;

  // Virtual destructor for polymorphic use
  virtual ~TrajectorySample() = default;

  // Forward declaration of Base used by methods below
  class Base;

  // Static kinematic interpolation between two samples (constant accel)
  static Base KinematicInterpolate(const TrajectorySample& start,
                                   const TrajectorySample& end,
                                   double t) {
    if (t <= 0.0) {
      return Base{start};
    } else if (t >= 1.0) {
      return Base{end};
    }

    // Interpolated delta time between start and end timestamps
    const auto interpT = wpi::util::Lerp(start.timestamp, end.timestamp, t);

    // Interpolate acceleration
    ChassisAccelerations newAccel{
        wpi::util::Lerp(start.acceleration.ax, end.acceleration.ax, t),
        wpi::util::Lerp(start.acceleration.ay, end.acceleration.ay, t),
        wpi::util::Lerp(start.acceleration.alpha, end.acceleration.alpha, t)};

    // v_{k+1} = v_k + a_k * dt
    ChassisSpeeds newVel{start.velocity.vx + start.acceleration.ax * interpT,
                         start.velocity.vy + start.acceleration.ay * interpT,
                         start.velocity.omega + start.acceleration.alpha * interpT};

    // x_{k+1} = x_k + v_k * dt + 0.5 a (dt)^2
    Pose2d newPose{
        start.pose.Translation().X() + start.velocity.vx * interpT +
            0.5 * start.acceleration.ax * interpT * interpT,
        start.pose.Translation().Y() + start.velocity.vy * interpT +
            0.5 * start.acceleration.ay * interpT * interpT,
        Rotation2d{start.pose.Rotation().Radians() + start.velocity.omega * interpT +
                   0.5 * start.acceleration.alpha * interpT * interpT}};

    return Base{interpT, newPose, newVel, newAccel};
  }

  // Pure virtuals to construct a new object of the same runtime type with modifications
  virtual std::unique_ptr<TrajectorySample> Transform(
      const Transform2d& transform) const = 0;

  virtual std::unique_ptr<TrajectorySample> RelativeTo(
      const Pose2d& other) const = 0;

  virtual std::unique_ptr<TrajectorySample> WithNewTimestamp(
      seconds_t newTimestamp) const = 0;

  // A base concrete sample type with no additional fields
  class Base : public TrajectorySample {
   public:
    using TrajectorySample::TrajectorySample;

    constexpr Base(const seconds_t time, const Pose2d& p, const ChassisSpeeds& v,
                             ChassisAccelerations a)
      : TrajectorySample{time, p, v, a} {}

    // Copy from any TrajectorySample
    explicit Base(const TrajectorySample& s)
        : TrajectorySample{s.timestamp, s.pose, s.velocity, s.acceleration} {}

    std::unique_ptr<TrajectorySample> Transform(
        const Transform2d& transform) const override {
      return std::make_unique<Base>(timestamp, pose.TransformBy(transform),
                                    velocity, acceleration);
    }

    std::unique_ptr<TrajectorySample> RelativeTo(
        const Pose2d& other) const override {
      return std::make_unique<Base>(timestamp, pose.RelativeTo(other), velocity,
                                    acceleration);
    }

    std::unique_ptr<TrajectorySample> WithNewTimestamp(
        seconds_t newTimestamp) const override {
      return std::make_unique<Base>(newTimestamp, pose, velocity, acceleration);
    }
  };
};

}  // namespace wpi::math

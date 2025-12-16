// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/units/time.hpp"

namespace wpi::math {

class SplineSample : public TrajectorySample {
 public:
  double curvature = 0.0;  // 1/m

  constexpr SplineSample() = default;

  constexpr SplineSample(wpi::units::second_t time, const Pose2d& pose,
                         const ChassisSpeeds& vel,
                         const ChassisAccelerations& acc, double curvature)
      : TrajectorySample(time, pose, vel, acc), curvature(curvature) {}

  // Construct from seconds as double for convenience
  explicit constexpr SplineSample(double timeSeconds, const Pose2d& pose,
                                  double linearVelocity,
                                  double linearAcceleration,
                                  double curvature)
      : TrajectorySample(wpi::units::second_t{timeSeconds}, pose,
                         ChassisSpeeds{wpi::units::meters_per_second_t{
                                           linearVelocity},
                                       0_mps,
                                       wpi::units::radians_per_second_t{
                                           linearVelocity * curvature}},
                         ChassisAccelerations{
                             wpi::units::meters_per_second_squared_t{
                                 linearAcceleration},
                             0_mps_sq,
                             wpi::units::radians_per_second_squared_t{
                                 linearAcceleration * curvature}}),
        curvature(curvature) {}

  // Construct by converting from a generic sample; curvature = omega / vx
  explicit constexpr SplineSample(const TrajectorySample& sample)
      : TrajectorySample(sample),
        curvature(sample.velocity.vx.value() == 0.0
                      ? (sample.velocity.omega.value() / 1e-9)
                      : (sample.velocity.omega.value() /
                         sample.velocity.vx.value())) {}

  std::unique_ptr<TrajectorySample> Transform(
      const Transform2d& transform) const override {
    return std::make_unique<SplineSample>(timestamp,
                                          pose.TransformBy(transform), velocity,
                                          acceleration, curvature);
  }

  std::unique_ptr<TrajectorySample> RelativeTo(
      const Pose2d& other) const override {
    return std::make_unique<SplineSample>(timestamp, pose.RelativeTo(other),
                                          velocity, acceleration, curvature);
  }

  std::unique_ptr<TrajectorySample> WithNewTimestamp(
      wpi::units::second_t newTimestamp) const override {
    return std::make_unique<SplineSample>(newTimestamp, pose, velocity,
                                          acceleration, curvature);
  }
};

}  // namespace wpi::math

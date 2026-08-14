// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/DifferentialTrajectory.hpp"

#include <cstddef>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <gcem.hpp>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/math/trajectory/DifferentialSample.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::math;

DifferentialSample DifferentialTrajectory::Interpolate(
    const DifferentialSample& start, const DifferentialSample& end,
    double t) const {
  wpi::units::second_t interpTime = wpi::util::Lerp(start.time, end.time, t);
  auto interpDt = interpTime - start.time;

  // The integration state holds wheel velocities (vₗ, vᵣ), which are
  // frame-invariant; the field-relative chassis velocity is reconstructed from
  // the integrated wheel velocities and heading below.
  Eigen::Vector<double, 6> initialState{
      {start.pose.X().value()},
      {start.pose.Y().value()},
      {start.pose.Rotation().Radians().value()},
      {start.leftVelocity.value()},
      {start.rightVelocity.value(), start.velocity.omega.value()}};

  // Wheel and angular accelerations are computed by finite difference between
  // the two samples (frame-independent, so no kinematics/trackwidth is needed).
  double segmentDt = (end.time - start.time).value();
  double leftAccel =
      segmentDt == 0
          ? 0
          : (end.leftVelocity - start.leftVelocity).value() / segmentDt;
  double rightAccel =
      segmentDt == 0
          ? 0
          : (end.rightVelocity - start.rightVelocity).value() / segmentDt;
  double angularAccel =
      segmentDt == 0
          ? 0
          : (end.velocity.omega - start.velocity.omega).value() / segmentDt;

  Eigen::Vector<double, 3> initialInput{
      {leftAccel}, {rightAccel}, {angularAccel}};

  // Integrate state derivatives [vₗ, vᵣ, ω, aₗ, aᵣ, α] to new states [x, y, θ,
  // vₗ, vᵣ, ω]
  auto dynamics = [&](const Eigen::Vector<double, 6>& state,
                      const Eigen::Vector<double, 3>& input) {
    double theta = state(2);
    double vl = state(3);
    double vr = state(4);
    double omega = state(5);
    double leftAccel = input(0);
    double rightAccel = input(1);
    double alpha = input(2);

    double v = (vl + vr) / 2.0;

    return Eigen::Vector<double, 6>{{v * gcem::cos(theta)},
                                    {v * gcem::sin(theta)},
                                    {omega},
                                    {leftAccel},
                                    {rightAccel},
                                    {alpha}};
  };

  Eigen::Vector<double, 6> endState =
      wpi::math::RKDP(dynamics, initialState, initialInput, interpDt);

  double x = endState(0);
  double y = endState(1);
  double theta = endState(2);
  double vl = endState(3);
  double vr = endState(4);
  double vx = (vl + vr) / 2.0;
  double omega = endState(5);

  auto ax = wpi::util::Lerp(start.acceleration.ax, end.acceleration.ax, t);
  auto ay = wpi::util::Lerp(start.acceleration.ay, end.acceleration.ay, t);
  auto alpha =
      wpi::util::Lerp(start.acceleration.alpha, end.acceleration.alpha, t);

  Rotation2d heading{wpi::units::radian_t{theta}};

  // Reconstruct the field-relative velocity from robot-relative forward
  // velocity.
  ChassisVelocities fieldVelocity = ChassisVelocities{
      wpi::units::meters_per_second_t{vx}, 0_mps,
      wpi::units::radians_per_second_t{
          omega}}.ToFieldRelative(heading);

  return {interpTime,
          Pose2d{wpi::units::meter_t{x}, wpi::units::meter_t{y}, heading},
          fieldVelocity,
          ChassisAccelerations{wpi::units::meters_per_second_squared_t{ax},
                               wpi::units::meters_per_second_squared_t{ay},
                               wpi::units::radians_per_second_squared_t{alpha}},
          wpi::units::meters_per_second_t{vl},
          wpi::units::meters_per_second_t{vr}};
}

DifferentialTrajectory DifferentialTrajectory::TransformBy(
    const Transform2d& transform) const {
  const Pose2d& firstPose = Start().pose;
  Pose2d transformedFirstPose = firstPose.TransformBy(transform);

  // The whole trajectory is rigidly rotated by the transform's rotation, so the
  // field-relative velocities and accelerations rotate by the same amount.
  const Rotation2d& rotation = transform.Rotation();

  std::vector<DifferentialSample> transformedSamples;
  transformedSamples.reserve(Samples().size());

  // Transform first sample
  transformedSamples.push_back(
      DifferentialSample(Start().time, transformedFirstPose,
                         Start().velocity.ToFieldRelative(rotation),
                         Start().acceleration.ToFieldRelative(rotation),
                         Start().leftVelocity, Start().rightVelocity));

  // Transform remaining samples
  for (size_t i = 1; i < Samples().size(); ++i) {
    const auto& sample = Samples()[i];
    transformedSamples.push_back(DifferentialSample(
        sample.time, transformedFirstPose + (sample.pose - firstPose),
        sample.velocity.ToFieldRelative(rotation),
        sample.acceleration.ToFieldRelative(rotation), sample.leftVelocity,
        sample.rightVelocity));
  }

  return DifferentialTrajectory(std::move(transformedSamples));
}

DifferentialTrajectory DifferentialTrajectory::RelativeTo(
    const Pose2d& pose) const {
  return DifferentialTrajectory{this->RelativeSamples(pose)};
}

DifferentialTrajectory DifferentialTrajectory::Concatenate(
    const DifferentialTrajectory& other) const {
  return DifferentialTrajectory{this->ConcatenateSamples(other.Samples())};
}

void wpi::math::to_json(wpi::util::json& json,
                        const DifferentialTrajectory& trajectory) {
  json = wpi::util::json::object("samples", trajectory.Samples());
}

void wpi::math::from_json(const wpi::util::json& json,
                          DifferentialTrajectory& trajectory) {
  std::vector<DifferentialSample> samples;
  from_json(json.at("samples"), samples);
  trajectory = DifferentialTrajectory{std::move(samples)};
}

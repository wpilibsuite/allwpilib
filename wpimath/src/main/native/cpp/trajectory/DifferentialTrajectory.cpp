// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/DifferentialTrajectory.hpp"

#include <utility>
#include <vector>

#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::math;

DifferentialSample DifferentialTrajectory::Interpolate(
    const DifferentialSample& start, const DifferentialSample& end,
    double t) const {
  wpi::units::second_t interpTime =
      wpi::util::Lerp(start.timestamp, end.timestamp, t);
  auto interpDt = interpTime - start.timestamp;

  Eigen::Vector<double, 6> initialState;
  initialState << start.pose.X().value(), start.pose.Y().value(),
      start.pose.Rotation().Radians().value(), start.velocity.vx.value(),
      start.velocity.vy.value(), start.velocity.omega.value();

  Eigen::Vector3d initialInput;
  initialInput << start.acceleration.ax.value(), start.acceleration.ay.value(),
      start.acceleration.alpha.value();

  // Integrate state derivatives [vₗ, vᵣ, ω, aₗ, aᵣ, α] to new states [x, y, θ,
  // vₗ, vᵣ, ω]
  auto dynamics = [&](const Eigen::Vector<double, 6>& state,
                      const Eigen::Vector3d& input) {
    double theta = state(2);
    double vl = state(3);
    double vr = state(4);
    double omega = state(5);
    double leftAccel = input(0);
    double rightAccel = input(1);
    double alpha = input(2);

    double v = (vl + vr) / 2.0;

    Eigen::Vector<double, 6> output;
    output << v * std::cos(theta), v * std::sin(theta), omega, leftAccel,
        rightAccel, alpha;
    return output;
  };

  Eigen::Vector<double, 6> endState =
      wpi::math::RKDP(dynamics, initialState, initialInput, interpDt);

  double x = endState(0);
  double y = endState(1);
  double theta = endState(2);
  double vl = endState(3);
  double vr = endState(4);
  double vx = (vl + vr) / 2.0;
  double vy = 0.0;
  double omega = endState(5);

  auto ax = wpi::util::Lerp(start.acceleration.ax, end.acceleration.ax, t);
  auto ay = wpi::util::Lerp(start.acceleration.ay, end.acceleration.ay, t);
  auto alpha =
      wpi::util::Lerp(start.acceleration.alpha, end.acceleration.alpha, t);

  return {interpTime,
          Pose2d{wpi::units::meter_t{x}, wpi::units::meter_t{y},
                 Rotation2d{wpi::units::radian_t{theta}}},
          ChassisSpeeds{wpi::units::meters_per_second_t{vx},
                        wpi::units::meters_per_second_t{vy},
                        wpi::units::radians_per_second_t{omega}},
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

  std::vector<DifferentialSample> transformedSamples;
  transformedSamples.reserve(Samples().size());

  // Transform first sample
  transformedSamples.push_back(DifferentialSample(
      Start().timestamp, transformedFirstPose, Start().velocity,
      Start().acceleration, Start().leftSpeed, Start().rightSpeed));

  // Transform remaining samples
  for (size_t i = 1; i < Samples().size(); ++i) {
    const auto& sample = Samples()[i];
    transformedSamples.push_back(DifferentialSample(
        sample.timestamp, transformedFirstPose + (sample.pose - firstPose),
        sample.velocity, sample.acceleration, sample.leftSpeed,
        sample.rightSpeed));
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
  json = wpi::util::json{{"samples", trajectory.Samples()}};
}

void wpi::math::from_json(const wpi::util::json& json,
                          DifferentialTrajectory& trajectory) {
  auto samples = json.at("samples").get<std::vector<DifferentialSample>>();
  trajectory = DifferentialTrajectory{std::move(samples)};
}

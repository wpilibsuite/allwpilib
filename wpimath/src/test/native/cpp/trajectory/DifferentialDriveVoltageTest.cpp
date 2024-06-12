// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "frc/trajectory/constraint/DifferentialDriveVoltageConstraint.h"
#include "trajectory/TestTrajectory.h"
#include "units/acceleration.h"
#include "units/length.h"
#include "units/time.h"
#include "units/velocity.h"
#include "units/voltage.h"

using namespace frc;

TEST(DifferentialDriveVoltageConstraintTest, Constraint) {
  constexpr units::second_t dt = 20_ms;

  // Pick an unreasonably large kA to ensure the constraint has to do some work
  SimpleMotorFeedforward<units::meter> feedforward{1_V, 1_V / 1_mps,
                                                   3_V / 1_mps_sq, dt};
  const DifferentialDriveKinematics kinematics{0.5_m};
  const auto maxVoltage = 10_V;

  auto config = TrajectoryConfig(12_fps, 12_fps_sq);
  config.AddConstraint(
      DifferentialDriveVoltageConstraint(feedforward, kinematics, maxVoltage));

  auto trajectory = TestTrajectory::GetTrajectory(config);

  units::second_t time = 0_s;
  units::second_t duration = trajectory.TotalTime();

  while (time < duration) {
    const Trajectory::State currentPoint = trajectory.Sample(time);
    const Trajectory::State nextPoint = trajectory.Sample(time + dt);
    time += dt;

    const ChassisSpeeds currentChassisSpeeds{
        currentPoint.velocity, 0_mps,
        currentPoint.velocity * currentPoint.curvature};
    auto [currentLeft, currentRight] =
        kinematics.ToWheelSpeeds(currentChassisSpeeds);

    const ChassisSpeeds nextChassisSpeeds{
        nextPoint.velocity, 0_mps, nextPoint.velocity * nextPoint.curvature};
    auto [nextLeft, nextRight] = kinematics.ToWheelSpeeds(nextChassisSpeeds);

    auto leftVoltage = feedforward.Calculate(currentLeft, nextLeft);
    auto rightVoltage = feedforward.Calculate(currentRight, nextRight);

    EXPECT_LT(leftVoltage.value(), (maxVoltage + 0.05_V).value());
    EXPECT_GT(leftVoltage.value(), (-maxVoltage - 0.05_V).value());
    EXPECT_LT(rightVoltage.value(), (maxVoltage + 0.05_V).value());
    EXPECT_GT(rightVoltage.value(), (-maxVoltage - 0.05_V).value());
  }
}

TEST(DifferentialDriveVoltageConstraintTest, HighCurvature) {
  constexpr units::second_t dt = 20_ms;

  SimpleMotorFeedforward<units::meter> feedforward{1_V, 1_V / 1_mps,
                                                   3_V / 1_mps_sq, dt};
  // Large trackwidth - need to test with radius of curvature less than half of
  // trackwidth
  const DifferentialDriveKinematics kinematics{3_m};
  const auto maxVoltage = 10_V;

  auto config = TrajectoryConfig(12_fps, 12_fps_sq);
  config.AddConstraint(
      DifferentialDriveVoltageConstraint(feedforward, kinematics, maxVoltage));

  EXPECT_NO_FATAL_FAILURE(TrajectoryGenerator::GenerateTrajectory(
      Pose2d{1_m, 0_m, 90_deg}, std::vector<Translation2d>{},
      Pose2d{0_m, 1_m, 180_deg}, config));

  config.SetReversed(true);

  EXPECT_NO_FATAL_FAILURE(TrajectoryGenerator::GenerateTrajectory(
      Pose2d{0_m, 1_m, 180_deg}, std::vector<Translation2d>{},
      Pose2d{1_m, 0_m, 90_deg}, config));
}

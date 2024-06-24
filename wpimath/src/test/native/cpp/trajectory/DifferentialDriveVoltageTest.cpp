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
  // Pick an unreasonably large kA to ensure the constraint has to do some work
  SimpleMotorFeedforward<units::meter> feedforward{1_V, 1_V / 1_mps,
                                                   3_V / 1_mps_sq};
  const DifferentialDriveKinematics kinematics{0.5_m};
  const auto maxVoltage = 10_V;

  auto config = TrajectoryConfig(12_fps, 12_fps_sq);
  config.AddConstraint(
      DifferentialDriveVoltageConstraint(feedforward, kinematics, maxVoltage));

  auto trajectory = TestTrajectory::GetTrajectory(config);

  units::second_t time = 0_s;
  units::second_t dt = 20_ms;
  units::second_t duration = trajectory.TotalTime();

  while (time < duration) {
    const Trajectory::State point = trajectory.Sample(time);
    time += dt;

    const ChassisSpeeds chassisSpeeds{point.velocity, 0_mps,
                                      point.velocity * point.curvature};

    auto [left, right] = kinematics.ToWheelSpeeds(chassisSpeeds);
    auto acceleration = point.acceleration;
    // Not really a strictly-correct test as we're using the chassis accel
    // instead of the wheel accel, but much easier than doing it "properly" and
    // a reasonable check anyway
    EXPECT_TRUE(feedforward.Calculate(left, left + acceleration * dt) <
                maxVoltage + 0.05_V);
    EXPECT_TRUE(feedforward.Calculate(left, left + acceleration * dt) >
                -maxVoltage - 0.05_V);
    EXPECT_TRUE(feedforward.Calculate(right,

                                      right + acceleration * dt) <
                maxVoltage + 0.05_V);
    EXPECT_TRUE(feedforward.Calculate(right, right + acceleration * dt) >
                -maxVoltage - 0.05_V);
  }
}

TEST(DifferentialDriveVoltageConstraintTest, HighCurvature) {
  SimpleMotorFeedforward<units::meter> feedforward{1_V, 1_V / 1_mps,
                                                   3_V / 1_mps_sq};
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

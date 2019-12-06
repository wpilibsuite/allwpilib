/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <memory>
#include <vector>

#include <units/units.h>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/trajectory/constraint/DifferentialDriveVoltageConstraint.h"
#include "gtest/gtest.h"
#include "trajectory/TestTrajectory.h"

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

    // Not really a strictly-correct test as we're using the chassis accel
    // instead of the wheel accel, but much easier than doing it "properly" and
    // a reasonable check anyway
    EXPECT_TRUE(feedforward.Calculate(left, point.acceleration) <
                maxVoltage + 0.05_V);
    EXPECT_TRUE(feedforward.Calculate(left, point.acceleration) >
                -maxVoltage - 0.05_V);
    EXPECT_TRUE(feedforward.Calculate(right, point.acceleration) <
                maxVoltage + 0.05_V);
    EXPECT_TRUE(feedforward.Calculate(right, point.acceleration) >
                -maxVoltage - 0.05_V);
  }
}

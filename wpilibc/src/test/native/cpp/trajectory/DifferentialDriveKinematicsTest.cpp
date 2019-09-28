/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <memory>
#include <vector>

#include <units/units.h>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/trajectory/constraint/DifferentialDriveKinematicsConstraint.h"
#include "gtest/gtest.h"
#include "trajectory/TestTrajectory.h"

using namespace frc;

TEST(DifferentialDriveKinematicsConstraintTest, Constraint) {
  const auto maxVelocity = 12_fps;
  const DifferentialDriveKinematics kinematics{27_in};

  std::vector<std::unique_ptr<TrajectoryConstraint>> constraints;
  constraints.emplace_back(
      std::make_unique<DifferentialDriveKinematicsConstraint>(kinematics,
                                                              maxVelocity));

  auto trajectory = TestTrajectory::GetTrajectory(std::move(constraints));

  units::second_t time = 0_s;
  units::second_t dt = 20_ms;
  units::second_t duration = trajectory.TotalTime();

  while (time < duration) {
    const Trajectory::State point = trajectory.Sample(time);
    time += dt;

    const ChassisSpeeds chassisSpeeds{point.velocity, 0_mps,
                                      point.velocity * point.curvature};

    auto [left, right] = kinematics.ToWheelSpeeds(chassisSpeeds);

    EXPECT_TRUE(left < maxVelocity + 0.05_mps);
    EXPECT_TRUE(right < maxVelocity + 0.05_mps);
  }
}

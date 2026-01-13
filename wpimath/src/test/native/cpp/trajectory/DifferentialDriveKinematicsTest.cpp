// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/math/trajectory/DifferentialSample.hpp"
#include "wpi/math/trajectory/TestTrajectory.hpp"
#include "wpi/math/trajectory/constraint/DifferentialDriveKinematicsConstraint.hpp"
#include "wpi/units/time.hpp"

using namespace wpi::math;

TEST(DifferentialDriveKinematicsConstraintTest, Constraint) {
  const auto maxVelocity = 12_fps;
  const DifferentialDriveKinematics kinematics{27_in};

  auto config = TrajectoryConfig(12_fps, 12_fps_sq);
  config.AddConstraint(
      DifferentialDriveKinematicsConstraint(kinematics, maxVelocity));

  auto trajectory = TestTrajectory::GetTrajectory(config);

  wpi::units::second_t time = 0_s;
  wpi::units::second_t dt = 20_ms;
  wpi::units::second_t duration = trajectory.TotalTime();

  while (time < duration) {
    const SplineSample point = trajectory.SampleAt(time);
    time += dt;

    const DifferentialSample differentialSample{point, kinematics};

    EXPECT_TRUE(differentialSample.leftSpeed < maxVelocity + 0.05_mps);
    EXPECT_TRUE(differentialSample.rightSpeed < maxVelocity + 0.05_mps);
  }
}

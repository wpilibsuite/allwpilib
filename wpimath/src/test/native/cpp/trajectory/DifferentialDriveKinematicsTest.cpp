// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"

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

  for (auto t = 0_s; t < trajectory.Duration(); t += 20_ms) {
    auto point = trajectory.SampleAt(t);

    const DifferentialSample differentialSample{point, kinematics};

    EXPECT_TRUE(differentialSample.leftSpeed < maxVelocity + 0.05_mps);
    EXPECT_TRUE(differentialSample.rightSpeed < maxVelocity + 0.05_mps);
  }
}

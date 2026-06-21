// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/DifferentialSample.hpp"
#include "wpi/math/trajectory/TestTrajectory.hpp"
#include "wpi/math/trajectory/constraint/DifferentialDriveKinematicsConstraint.hpp"
#include "wpi/units/time.hpp"

using namespace wpi::math;

TEST_CASE("DifferentialDriveKinematicsConstraintTest Constraint", "[wpimath]") {
  const auto maxVelocity = 12_fps;
  const DifferentialDriveKinematics kinematics{27_in};

  auto config = TrajectoryConfig(12_fps, 12_fps_sq);
  config.AddConstraint(
      DifferentialDriveKinematicsConstraint(kinematics, maxVelocity));

  auto trajectory = TestTrajectory::GetTrajectory(config);

  for (auto t = 0_s; t < trajectory.Duration(); t += 20_ms) {
    auto point = trajectory.SampleAt(t);

    const DifferentialSample differentialSample{point, kinematics};

    CHECK(differentialSample.leftVelocity < maxVelocity + 0.05_mps);
    CHECK(differentialSample.rightVelocity < maxVelocity + 0.05_mps);
  }
}

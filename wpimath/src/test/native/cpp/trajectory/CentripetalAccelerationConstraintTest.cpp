// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/constraint/CentripetalAccelerationConstraint.hpp"

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/TestDrivetrainSplineTrajectory.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/core.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

TEST_CASE("CentripetalAccelerationConstraintTest Constraint", "[wpimath]") {
  const auto maxCentripetalAcceleration = 7_fps2;

  auto config = TrajectoryConfig(12_fps, 12_fps2);
  config.AddConstraint(
      CentripetalAccelerationConstraint(maxCentripetalAcceleration));

  auto trajectory = TestDrivetrainSplineTrajectory::GetTrajectory(config);

  for (auto t = 0_s; t < trajectory.Duration(); t += 20_ms) {
    auto point = trajectory.SampleAt(t);
    auto centripetalAcceleration =
        wpi::units::pow<2>(point.ForwardVelocity()) * point.curvature / 1_rad;

    CHECK(centripetalAcceleration < maxCentripetalAcceleration + 0.05_mps2);
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

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

  wpi::units::second_t time = 0_s;
  wpi::units::second_t dt = 20_ms;
  wpi::units::second_t duration = trajectory.TotalTime();

  while (time < duration) {
    const Trajectory::State point = trajectory.Sample(time);
    time += dt;

    const ChassisVelocities chassisVelocities{point.velocity, 0_mps,
                                              point.velocity * point.curvature};

    auto [left, right] = kinematics.ToWheelVelocities(chassisVelocities);

    CHECK(left < maxVelocity + 0.05_mps);
    CHECK(right < maxVelocity + 0.05_mps);
  }
}

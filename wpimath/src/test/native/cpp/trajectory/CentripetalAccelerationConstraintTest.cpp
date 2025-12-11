// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/constraint/CentripetalAccelerationConstraint.hpp"

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/math/trajectory/TestTrajectory.hpp"
#include "wpi/math/trajectory/constraint/TrajectoryConstraint.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

TEST(CentripetalAccelerationConstraintTest, Constraint) {
  const auto maxCentripetalAcceleration = 7_fps_sq;

  auto config = TrajectoryConfig(12_fps, 12_fps_sq);
  config.AddConstraint(
      CentripetalAccelerationConstraint(maxCentripetalAcceleration));

  auto trajectory = TestTrajectory::GetTrajectory(config);

  wpi::units::second_t time = 0_s;
  wpi::units::second_t dt = 20_ms;
  wpi::units::second_t duration = trajectory.TotalTime();

  while (time < duration) {
    const Trajectory::State point = trajectory.Sample(time);
    time += dt;

    auto centripetalAcceleration =
        wpi::units::math::pow<2>(point.velocity) * point.curvature / 1_rad;

    EXPECT_TRUE(centripetalAcceleration <
                maxCentripetalAcceleration + 0.05_mps_sq);
  }
}

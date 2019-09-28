/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <memory>
#include <vector>

#include <units/units.h>

#include "frc/trajectory/constraint/CentripetalAccelerationConstraint.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"
#include "gtest/gtest.h"
#include "trajectory/TestTrajectory.h"

using namespace frc;

TEST(CentripetalAccelerationConstraintTest, Constraint) {
  const auto maxCentripetalAcceleration = 7_fps_sq;

  std::vector<std::unique_ptr<TrajectoryConstraint>> constraints;
  constraints.emplace_back(std::make_unique<CentripetalAccelerationConstraint>(
      maxCentripetalAcceleration));

  auto trajectory = TestTrajectory::GetTrajectory(std::move(constraints));

  units::second_t time = 0_s;
  units::second_t dt = 20_ms;
  units::second_t duration = trajectory.TotalTime();

  while (time < duration) {
    const Trajectory::State point = trajectory.Sample(time);
    time += dt;

    auto centripetalAcceleration =
        units::math::pow<2>(point.velocity) * point.curvature / 1_rad;

    EXPECT_TRUE(centripetalAcceleration <
                maxCentripetalAcceleration + 0.05_mps_sq);
  }
}

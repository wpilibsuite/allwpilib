// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "trajectory/TestTrajectory.h"
#include "units/acceleration.h"
#include "units/angle.h"
#include "units/length.h"
#include "units/velocity.h"
#include "wpimath/trajectory/constraint/EllipticalRegionConstraint.h"
#include "wpimath/trajectory/constraint/MaxVelocityConstraint.h"

using namespace wpimath;

TEST(EllipticalRegionConstraintTest, Constraint) {
  constexpr auto maxVelocity = 2_fps;
  constexpr wpimath::Ellipse2d ellipse{{5_ft, 2.5_ft, 180_deg}, 5_ft, 2.5_ft};

  auto config = TrajectoryConfig(13_fps, 13_fps_sq);
  config.AddConstraint(
      EllipticalRegionConstraint{ellipse, MaxVelocityConstraint{maxVelocity}});
  auto trajectory = TestTrajectory::GetTrajectory(config);

  bool exceededConstraintOutsideRegion = false;
  for (auto& point : trajectory.States()) {
    if (ellipse.Contains(point.pose.Translation())) {
      EXPECT_TRUE(units::math::abs(point.velocity) < maxVelocity + 0.05_mps);
    } else if (units::math::abs(point.velocity) >= maxVelocity + 0.05_mps) {
      exceededConstraintOutsideRegion = true;
    }
  }
  EXPECT_TRUE(exceededConstraintOutsideRegion);
}

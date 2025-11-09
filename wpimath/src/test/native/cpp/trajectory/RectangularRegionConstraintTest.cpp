// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/trajectory/TestTrajectory.hpp"
#include "wpi/math/trajectory/constraint/MaxVelocityConstraint.hpp"
#include "wpi/math/trajectory/constraint/RectangularRegionConstraint.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

TEST(RectangularRegionConstraintTest, Constraint) {
  constexpr auto maxVelocity = 2_fps;
  constexpr wpi::math::Rectangle2d rectangle{{1_ft, 1_ft}, {5_ft, 27_ft}};

  auto config = TrajectoryConfig(13_fps, 13_fps_sq);
  config.AddConstraint(RectangularRegionConstraint{
      rectangle, MaxVelocityConstraint{maxVelocity}});
  auto trajectory = TestTrajectory::GetTrajectory(config);

  bool exceededConstraintOutsideRegion = false;
  for (auto& point : trajectory.States()) {
    if (rectangle.Contains(point.pose.Translation())) {
      EXPECT_TRUE(wpi::units::math::abs(point.velocity) <
                  maxVelocity + 0.05_mps);
    } else if (wpi::units::math::abs(point.velocity) >=
               maxVelocity + 0.05_mps) {
      exceededConstraintOutsideRegion = true;
    }
  }
  EXPECT_TRUE(exceededConstraintOutsideRegion);
}

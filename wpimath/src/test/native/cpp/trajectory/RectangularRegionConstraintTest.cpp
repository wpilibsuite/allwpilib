// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/constraint/RectangularRegionConstraint.hpp"

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/shape/Rectangle2d.hpp"
#include "wpi/math/trajectory/TestDrivetrainSplineTrajectory.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/math/trajectory/constraint/MaxVelocityConstraint.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

TEST_CASE("RectangularRegionConstraintTest Constraint", "[wpimath]") {
  constexpr auto maxVelocity = 2_fps;
  constexpr wpi::math::Rectangle2d rectangle{{1_ft, 1_ft}, {5_ft, 27_ft}};

  auto config = TrajectoryConfig(13_fps, 13_fps2);
  config.AddConstraint(RectangularRegionConstraint{
      rectangle, MaxVelocityConstraint{maxVelocity}});
  auto trajectory = TestDrivetrainSplineTrajectory::GetTrajectory(config);

  bool exceededConstraintOutsideRegion = false;
  for (auto& point : trajectory.Samples()) {
    if (rectangle.Contains(point.pose.Translation())) {
      CHECK(wpi::units::abs(point.ForwardVelocity()) < maxVelocity + 0.05_mps);
    } else if (wpi::units::abs(point.ForwardVelocity()) >=
               maxVelocity + 0.05_mps) {
      exceededConstraintOutsideRegion = true;
    }
  }
  CHECK(exceededConstraintOutsideRegion);
}

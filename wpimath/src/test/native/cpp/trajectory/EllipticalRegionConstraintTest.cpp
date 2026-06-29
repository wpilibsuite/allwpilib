// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/constraint/EllipticalRegionConstraint.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/TestTrajectory.hpp"
#include "wpi/math/trajectory/constraint/MaxVelocityConstraint.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

TEST_CASE("EllipticalRegionConstraintTest Constraint", "[wpimath]") {
  constexpr auto maxVelocity = 2_fps;
  constexpr wpi::math::Ellipse2d ellipse{{5_ft, 2.5_ft, 180_deg}, 5_ft, 2.5_ft};

  auto config = TrajectoryConfig(13_fps, 13_fps_sq);
  config.AddConstraint(
      EllipticalRegionConstraint{ellipse, MaxVelocityConstraint{maxVelocity}});
  auto trajectory = TestTrajectory::GetTrajectory(config);

  bool exceededConstraintOutsideRegion = false;
  for (auto& point : trajectory.States()) {
    if (ellipse.Contains(point.pose.Translation())) {
      CHECK(wpi::units::math::abs(point.velocity) < maxVelocity + 0.05_mps);
    } else if (wpi::units::math::abs(point.velocity) >=
               maxVelocity + 0.05_mps) {
      exceededConstraintOutsideRegion = true;
    }
  }
  CHECK(exceededConstraintOutsideRegion);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/trajectory/constraint/MaxVelocityConstraint.h"
#include "frc/trajectory/constraint/RectangularRegionConstraint.h"
#include "gtest/gtest.h"
#include "trajectory/TestTrajectory.h"
#include "units/acceleration.h"
#include "units/length.h"
#include "units/math.h"
#include "units/velocity.h"

using namespace frc;

TEST(RectangularRegionConstraintTest, Constraint) {
  constexpr auto maxVelocity = 2_fps;

  auto config = TrajectoryConfig(13_fps, 13_fps_sq);
  MaxVelocityConstraint maxVelConstraint(maxVelocity);
  RectangularRegionConstraint regionConstraint(frc::Translation2d{1_ft, 1_ft},
                                               frc::Translation2d{5_ft, 27_ft},
                                               maxVelConstraint);
  config.AddConstraint(regionConstraint);

  auto trajectory = TestTrajectory::GetTrajectory(config);

  bool exceededConstraintOutsideRegion = false;
  for (auto& point : trajectory.States()) {
    if (regionConstraint.IsPoseInRegion(point.pose)) {
      EXPECT_TRUE(units::math::abs(point.velocity) < maxVelocity + 0.05_mps);
    } else if (units::math::abs(point.velocity) >= maxVelocity + 0.05_mps) {
      exceededConstraintOutsideRegion = true;
    }
  }

  EXPECT_TRUE(exceededConstraintOutsideRegion);
}

TEST(RectangularRegionConstraintTest, IsPoseInRegion) {
  constexpr auto maxVelocity = 2_fps;
  MaxVelocityConstraint maxVelConstraint(maxVelocity);
  RectangularRegionConstraint regionConstraint(frc::Translation2d{1_ft, 1_ft},
                                               frc::Translation2d{5_ft, 27_ft},
                                               maxVelConstraint);

  EXPECT_FALSE(regionConstraint.IsPoseInRegion(Pose2d{}));
  EXPECT_TRUE(regionConstraint.IsPoseInRegion(Pose2d{3_ft, 14.5_ft, 0_deg}));
}

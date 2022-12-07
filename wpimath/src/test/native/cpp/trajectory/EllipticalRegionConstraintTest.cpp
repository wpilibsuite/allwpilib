// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/trajectory/constraint/EllipticalRegionConstraint.h"
#include "frc/trajectory/constraint/MaxVelocityConstraint.h"
#include "gtest/gtest.h"
#include "trajectory/TestTrajectory.h"
#include "units/acceleration.h"
#include "units/angle.h"
#include "units/length.h"
#include "units/velocity.h"

using namespace frc;

TEST(EllipticalRegionConstraintTest, Constraint) {
  constexpr auto maxVelocity = 2_fps;

  auto config = TrajectoryConfig(13_fps, 13_fps_sq);
  MaxVelocityConstraint maxVelConstraint(maxVelocity);
  EllipticalRegionConstraint regionConstraint(
      frc::Translation2d{5_ft, 2.5_ft}, 10_ft, 5_ft, 180_deg, maxVelConstraint);
  config.AddConstraint(regionConstraint);

  auto trajectory = TestTrajectory::GetTrajectory(config);

  bool exceededConstraintOutsideRegion = false;
  for (auto& point : trajectory.States()) {
    auto translation = point.pose.Translation();
    if (translation.X() < 10_ft && translation.Y() < 5_ft) {
      EXPECT_TRUE(units::math::abs(point.velocity) < maxVelocity + 0.05_mps);
    } else if (units::math::abs(point.velocity) >= maxVelocity + 0.05_mps) {
      exceededConstraintOutsideRegion = true;
    }
  }

  EXPECT_TRUE(exceededConstraintOutsideRegion);
}

TEST(EllipticalRegionConstraintTest, IsPoseInRegion) {
  constexpr auto maxVelocity = 2_fps;
  MaxVelocityConstraint maxVelConstraint(maxVelocity);
  EllipticalRegionConstraint regionConstraintNoRotation(
      frc::Translation2d{1_ft, 1_ft}, 2_ft, 4_ft, 0_deg, maxVelConstraint);
  EXPECT_FALSE(regionConstraintNoRotation.IsPoseInRegion(
      frc::Pose2d{2.1_ft, 1_ft, 0_rad}));

  EllipticalRegionConstraint regionConstraintWithRotation(
      frc::Translation2d{1_ft, 1_ft}, 2_ft, 4_ft, 90_deg, maxVelConstraint);
  EXPECT_TRUE(regionConstraintWithRotation.IsPoseInRegion(
      frc::Pose2d{2.1_ft, 1_ft, 0_rad}));
}

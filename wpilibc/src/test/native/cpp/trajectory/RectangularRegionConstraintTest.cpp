/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <vector>

#include <units/units.h>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/trajectory/constraint/MaxVelocityConstraint.h"
#include "frc/trajectory/constraint/RectangularRegionConstraint.h"
#include "gtest/gtest.h"
#include "trajectory/TestTrajectory.h"

using namespace frc;

TEST(EllipticalRegionConstraintTest, Constraint) {
  constexpr auto maxVelocity = 2_fps;
  constexpr auto dt = 20_ms;

  auto config = TrajectoryConfig(13_fps, 13_fps_sq);
  MaxVelocityConstraint maxVelConstraint(maxVelocity);
  EllipticalRegionConstraint regionConstraint(frc::Translation2d{1_ft, 1_ft},
                                              frc::Translation2d{5_ft, 27_ft},
                                              &maxVelConstraint);
  config.AddConstraint(regionConstraint);

  auto trajectory = TestTrajectory::GetTrajectory(config);

  bool exceededConstraintOutsideRegion = false;
  for (auto time = 0_s; time < trajectory.TotalTime(); time += dt) {
    const Trajectory::State point = trajectory.Sample(time);

    auto translation = point.pose.Translation();
    if (regionConstraint.IsPoseInRegion(point.pose)) {
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
  EllipticalRegionConstraint regionConstraint(frc::Translation2d{1_ft, 1_ft},
                                              frc::Translation2d{5_ft, 27_ft},
                                              &maxVelConstraint);

  EXPECT_FALSE(regionConstraint.IsPoseInRegion(Pose2d()));
  EXPECT_TRUE(
      regionConstraint.IsPoseInRegion(Pose2d(3_ft, 14.5_ft, Rotation2d())));
}

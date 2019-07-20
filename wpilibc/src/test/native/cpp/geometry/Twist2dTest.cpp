/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "frc/geometry/Pose2d.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(Twist2dTest, Straight) {
  const Twist2d straight{5.0, 0.0, 0.0};
  const auto straightPose = Pose2d().Exp(straight);

  EXPECT_NEAR(straightPose.Translation().X(), 5.0, kEpsilon);
  EXPECT_NEAR(straightPose.Translation().Y(), 0.0, kEpsilon);
  EXPECT_NEAR(straightPose.Rotation().Radians(), 0.0, kEpsilon);
}

TEST(Twist2dTest, QuarterCircle) {
  const Twist2d quarterCircle{5.0 / 2.0 * 3.14159265358979323846, 0,
                              3.14159265358979323846 / 2.0};
  const auto quarterCirclePose = Pose2d().Exp(quarterCircle);

  EXPECT_NEAR(quarterCirclePose.Translation().X(), 5.0, kEpsilon);
  EXPECT_NEAR(quarterCirclePose.Translation().Y(), 5.0, kEpsilon);
  EXPECT_NEAR(quarterCirclePose.Rotation().Degrees(), 90.0, kEpsilon);
}

TEST(Twist2dTest, DiagonalNoDtheta) {
  const Twist2d diagonal{2.0, 2.0, 0.0};
  const auto diagonalPose = Pose2d().Exp(diagonal);

  EXPECT_NEAR(diagonalPose.Translation().X(), 2.0, kEpsilon);
  EXPECT_NEAR(diagonalPose.Translation().Y(), 2.0, kEpsilon);
  EXPECT_NEAR(diagonalPose.Rotation().Degrees(), 0.0, kEpsilon);
}

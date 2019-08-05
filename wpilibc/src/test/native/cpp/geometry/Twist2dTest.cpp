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
  const Twist2d straight{5.0_m, 0.0_m, 0.0_rad};
  const auto straightPose = Pose2d().Exp(straight);

  EXPECT_NEAR(straightPose.Translation().X().to<double>(), 5.0, kEpsilon);
  EXPECT_NEAR(straightPose.Translation().Y().to<double>(), 0.0, kEpsilon);
  EXPECT_NEAR(straightPose.Rotation().Radians().to<double>(), 0.0, kEpsilon);
}

TEST(Twist2dTest, QuarterCircle) {
  const Twist2d quarterCircle{5.0_m / 2.0 * 3.14159265358979323846, 0_m,
                              3.14159265358979323846_rad / 2.0};
  const auto quarterCirclePose = Pose2d().Exp(quarterCircle);

  EXPECT_NEAR(quarterCirclePose.Translation().X().to<double>(), 5.0, kEpsilon);
  EXPECT_NEAR(quarterCirclePose.Translation().Y().to<double>(), 5.0, kEpsilon);
  EXPECT_NEAR(quarterCirclePose.Rotation().Degrees().to<double>(), 90.0,
              kEpsilon);
}

TEST(Twist2dTest, DiagonalNoDtheta) {
  const Twist2d diagonal{2.0_m, 2.0_m, 0.0_deg};
  const auto diagonalPose = Pose2d().Exp(diagonal);

  EXPECT_NEAR(diagonalPose.Translation().X().to<double>(), 2.0, kEpsilon);
  EXPECT_NEAR(diagonalPose.Translation().Y().to<double>(), 2.0, kEpsilon);
  EXPECT_NEAR(diagonalPose.Rotation().Degrees().to<double>(), 0.0, kEpsilon);
}

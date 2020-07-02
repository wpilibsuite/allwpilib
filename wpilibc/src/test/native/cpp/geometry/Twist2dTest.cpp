/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include <wpi/math>

#include "frc/geometry/Pose2d.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(Twist2dTest, Straight) {
  const Twist2d straight{5.0_m, 0.0_m, 0.0_rad};
  const auto straightPose = Pose2d().Exp(straight);

  EXPECT_NEAR(straightPose.X().to<double>(), 5.0, kEpsilon);
  EXPECT_NEAR(straightPose.Y().to<double>(), 0.0, kEpsilon);
  EXPECT_NEAR(straightPose.Rotation().Radians().to<double>(), 0.0, kEpsilon);
}

TEST(Twist2dTest, QuarterCircle) {
  const Twist2d quarterCircle{5.0_m / 2.0 * wpi::math::pi, 0_m,
                              units::radian_t(wpi::math::pi / 2.0)};
  const auto quarterCirclePose = Pose2d().Exp(quarterCircle);

  EXPECT_NEAR(quarterCirclePose.X().to<double>(), 5.0, kEpsilon);
  EXPECT_NEAR(quarterCirclePose.Y().to<double>(), 5.0, kEpsilon);
  EXPECT_NEAR(quarterCirclePose.Rotation().Degrees().to<double>(), 90.0,
              kEpsilon);
}

TEST(Twist2dTest, DiagonalNoDtheta) {
  const Twist2d diagonal{2.0_m, 2.0_m, 0.0_deg};
  const auto diagonalPose = Pose2d().Exp(diagonal);

  EXPECT_NEAR(diagonalPose.X().to<double>(), 2.0, kEpsilon);
  EXPECT_NEAR(diagonalPose.Y().to<double>(), 2.0, kEpsilon);
  EXPECT_NEAR(diagonalPose.Rotation().Degrees().to<double>(), 0.0, kEpsilon);
}

TEST(Twist2dTest, Equality) {
  const Twist2d one{5.0_m, 1.0_m, 3.0_rad};
  const Twist2d two{5.0_m, 1.0_m, 3.0_rad};
  EXPECT_TRUE(one == two);
}

TEST(Twist2dTest, Inequality) {
  const Twist2d one{5.0_m, 1.0_m, 3.0_rad};
  const Twist2d two{5.0_m, 1.2_m, 3.0_rad};
  EXPECT_TRUE(one != two);
}

TEST(Twist2dTest, Pose2dLog) {
  const Pose2d end{5_m, 5_m, Rotation2d(90_deg)};
  const Pose2d start{};

  const auto twist = start.Log(end);

  EXPECT_NEAR(twist.dx.to<double>(), 5 / 2.0 * wpi::math::pi, kEpsilon);
  EXPECT_NEAR(twist.dy.to<double>(), 0.0, kEpsilon);
  EXPECT_NEAR(twist.dtheta.to<double>(), wpi::math::pi / 2.0, kEpsilon);
}

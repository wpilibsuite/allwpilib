// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>

#include "frc/geometry/Pose2d.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(Twist2dTest, Straight) {
  const Twist2d straight{5_m, 0_m, 0_rad};
  const auto straightPose = Pose2d{}.Exp(straight);

  EXPECT_DOUBLE_EQ(5.0, straightPose.X().value());
  EXPECT_DOUBLE_EQ(0.0, straightPose.Y().value());
  EXPECT_DOUBLE_EQ(0.0, straightPose.Rotation().Radians().value());
}

TEST(Twist2dTest, QuarterCircle) {
  const Twist2d quarterCircle{5_m / 2.0 * std::numbers::pi, 0_m,
                              units::radian_t{std::numbers::pi / 2.0}};
  const auto quarterCirclePose = Pose2d{}.Exp(quarterCircle);

  EXPECT_DOUBLE_EQ(5.0, quarterCirclePose.X().value());
  EXPECT_DOUBLE_EQ(5.0, quarterCirclePose.Y().value());
  EXPECT_DOUBLE_EQ(90.0, quarterCirclePose.Rotation().Degrees().value());
}

TEST(Twist2dTest, DiagonalNoDtheta) {
  const Twist2d diagonal{2_m, 2_m, 0_deg};
  const auto diagonalPose = Pose2d{}.Exp(diagonal);

  EXPECT_DOUBLE_EQ(2.0, diagonalPose.X().value());
  EXPECT_DOUBLE_EQ(2.0, diagonalPose.Y().value());
  EXPECT_DOUBLE_EQ(0.0, diagonalPose.Rotation().Degrees().value());
}

TEST(Twist2dTest, Equality) {
  const Twist2d one{5_m, 1_m, 3_rad};
  const Twist2d two{5_m, 1_m, 3_rad};
  EXPECT_TRUE(one == two);
}

TEST(Twist2dTest, Inequality) {
  const Twist2d one{5_m, 1_m, 3_rad};
  const Twist2d two{5_m, 1.2_m, 3_rad};
  EXPECT_TRUE(one != two);
}

TEST(Twist2dTest, Pose2dLog) {
  const Pose2d end{5_m, 5_m, 90_deg};
  const Pose2d start;

  const auto twist = start.Log(end);

  Twist2d expected{units::meter_t{5.0 / 2.0 * std::numbers::pi}, 0_m,
                   units::radian_t{std::numbers::pi / 2.0}};
  EXPECT_EQ(expected, twist);

  // Make sure computed twist gives back original end pose
  const auto reapplied = start.Exp(twist);
  EXPECT_EQ(end, reapplied);
}

TEST(Twist2dTest, Constexpr) {
  constexpr Twist2d defaultCtor;
  constexpr Twist2d componentCtor{1_m, 2_m, 3_rad};
  constexpr auto multiplied = componentCtor * 2;

  static_assert(defaultCtor.dx == 0_m);
  static_assert(componentCtor.dy == 2_m);
  static_assert(multiplied.dtheta == 6_rad);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include "frc/geometry/Translation2d.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(Translation2dTest, Sum) {
  const Translation2d one{1.0_m, 3.0_m};
  const Translation2d two{2.0_m, 5.0_m};

  const auto sum = one + two;

  EXPECT_NEAR(sum.X().value(), 3.0, kEpsilon);
  EXPECT_NEAR(sum.Y().value(), 8.0, kEpsilon);
}

TEST(Translation2dTest, Difference) {
  const Translation2d one{1.0_m, 3.0_m};
  const Translation2d two{2.0_m, 5.0_m};

  const auto difference = one - two;

  EXPECT_NEAR(difference.X().value(), -1.0, kEpsilon);
  EXPECT_NEAR(difference.Y().value(), -2.0, kEpsilon);
}

TEST(Translation2dTest, RotateBy) {
  const Translation2d another{3.0_m, 0.0_m};
  const auto rotated = another.RotateBy(Rotation2d(90.0_deg));

  EXPECT_NEAR(rotated.X().value(), 0.0, kEpsilon);
  EXPECT_NEAR(rotated.Y().value(), 3.0, kEpsilon);
}

TEST(Translation2dTest, Multiplication) {
  const Translation2d original{3.0_m, 5.0_m};
  const auto mult = original * 3;

  EXPECT_NEAR(mult.X().value(), 9.0, kEpsilon);
  EXPECT_NEAR(mult.Y().value(), 15.0, kEpsilon);
}

TEST(Translation2dTest, Division) {
  const Translation2d original{3.0_m, 5.0_m};
  const auto div = original / 2;

  EXPECT_NEAR(div.X().value(), 1.5, kEpsilon);
  EXPECT_NEAR(div.Y().value(), 2.5, kEpsilon);
}

TEST(Translation2dTest, Norm) {
  const Translation2d one{3.0_m, 5.0_m};
  EXPECT_NEAR(one.Norm().value(), std::hypot(3, 5), kEpsilon);
}

TEST(Translation2dTest, Distance) {
  const Translation2d one{1_m, 1_m};
  const Translation2d two{6_m, 6_m};
  EXPECT_NEAR(one.Distance(two).value(), 5 * std::sqrt(2), kEpsilon);
}

TEST(Translation2dTest, UnaryMinus) {
  const Translation2d original{-4.5_m, 7_m};
  const auto inverted = -original;

  EXPECT_NEAR(inverted.X().value(), 4.5, kEpsilon);
  EXPECT_NEAR(inverted.Y().value(), -7, kEpsilon);
}

TEST(Translation2dTest, Equality) {
  const Translation2d one{9_m, 5.5_m};
  const Translation2d two{9_m, 5.5_m};
  EXPECT_TRUE(one == two);
}

TEST(Translation2dTest, Inequality) {
  const Translation2d one{9_m, 5.5_m};
  const Translation2d two{9_m, 5.7_m};
  EXPECT_TRUE(one != two);
}

TEST(Translation2dTest, PolarConstructor) {
  Translation2d one{std::sqrt(2) * 1_m, Rotation2d(45_deg)};
  EXPECT_NEAR(one.X().value(), 1.0, kEpsilon);
  EXPECT_NEAR(one.Y().value(), 1.0, kEpsilon);

  Translation2d two{2_m, Rotation2d(60_deg)};
  EXPECT_NEAR(two.X().value(), 1.0, kEpsilon);
  EXPECT_NEAR(two.Y().value(), std::sqrt(3.0), kEpsilon);
}

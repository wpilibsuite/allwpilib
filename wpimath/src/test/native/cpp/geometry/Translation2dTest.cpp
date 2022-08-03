// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include "frc/geometry/Translation2d.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(Translation2dTest, Sum) {
  const Translation2d one{1_m, 3_m};
  const Translation2d two{2_m, 5_m};

  const auto sum = one + two;

  EXPECT_DOUBLE_EQ(3.0, sum.X().value());
  EXPECT_DOUBLE_EQ(8.0, sum.Y().value());
}

TEST(Translation2dTest, Difference) {
  const Translation2d one{1_m, 3_m};
  const Translation2d two{2_m, 5_m};

  const auto difference = one - two;

  EXPECT_DOUBLE_EQ(-1.0, difference.X().value());
  EXPECT_DOUBLE_EQ(-2.0, difference.Y().value());
}

TEST(Translation2dTest, RotateBy) {
  const Translation2d another{3_m, 0_m};
  const auto rotated = another.RotateBy(Rotation2d(90_deg));

  EXPECT_NEAR(0.0, rotated.X().value(), 1e-9);
  EXPECT_DOUBLE_EQ(3.0, rotated.Y().value());
}

TEST(Translation2dTest, Multiplication) {
  const Translation2d original{3_m, 5_m};
  const auto mult = original * 3;

  EXPECT_DOUBLE_EQ(9.0, mult.X().value());
  EXPECT_DOUBLE_EQ(15.0, mult.Y().value());
}

TEST(Translation2dTest, Division) {
  const Translation2d original{3_m, 5_m};
  const auto div = original / 2;

  EXPECT_DOUBLE_EQ(1.5, div.X().value());
  EXPECT_DOUBLE_EQ(2.5, div.Y().value());
}

TEST(Translation2dTest, Norm) {
  const Translation2d one{3_m, 5_m};
  EXPECT_DOUBLE_EQ(std::hypot(3.0, 5.0), one.Norm().value());
}

TEST(Translation2dTest, Distance) {
  const Translation2d one{1_m, 1_m};
  const Translation2d two{6_m, 6_m};
  EXPECT_DOUBLE_EQ(5.0 * std::sqrt(2.0), one.Distance(two).value());
}

TEST(Translation2dTest, UnaryMinus) {
  const Translation2d original{-4.5_m, 7_m};
  const auto inverted = -original;

  EXPECT_DOUBLE_EQ(4.5, inverted.X().value());
  EXPECT_DOUBLE_EQ(-7.0, inverted.Y().value());
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
  Translation2d one{std::sqrt(2) * 1_m, Rotation2d{45_deg}};
  EXPECT_DOUBLE_EQ(1.0, one.X().value());
  EXPECT_DOUBLE_EQ(1.0, one.Y().value());

  Translation2d two{2_m, Rotation2d{60_deg}};
  EXPECT_DOUBLE_EQ(1.0, two.X().value());
  EXPECT_DOUBLE_EQ(std::sqrt(3.0), two.Y().value());
}

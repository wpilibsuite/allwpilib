/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "frc/geometry/Translation2d.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(Translation2dTest, Sum) {
  const Translation2d one{1.0_m, 3.0_m};
  const Translation2d two{2.0_m, 5.0_m};

  const auto sum = one + two;

  EXPECT_NEAR(sum.X().to<double>(), 3.0, kEpsilon);
  EXPECT_NEAR(sum.Y().to<double>(), 8.0, kEpsilon);
}

TEST(Translation2dTest, Difference) {
  const Translation2d one{1.0_m, 3.0_m};
  const Translation2d two{2.0_m, 5.0_m};

  const auto difference = one - two;

  EXPECT_NEAR(difference.X().to<double>(), -1.0, kEpsilon);
  EXPECT_NEAR(difference.Y().to<double>(), -2.0, kEpsilon);
}

TEST(Translation2dTest, RotateBy) {
  const Translation2d another{3.0_m, 0.0_m};
  const auto rotated = another.RotateBy(Rotation2d(90.0_deg));

  EXPECT_NEAR(rotated.X().to<double>(), 0.0, kEpsilon);
  EXPECT_NEAR(rotated.Y().to<double>(), 3.0, kEpsilon);
}

TEST(Translation2dTest, Multiplication) {
  const Translation2d original{3.0_m, 5.0_m};
  const auto mult = original * 3;

  EXPECT_NEAR(mult.X().to<double>(), 9.0, kEpsilon);
  EXPECT_NEAR(mult.Y().to<double>(), 15.0, kEpsilon);
}

TEST(Translation2d, Division) {
  const Translation2d original{3.0_m, 5.0_m};
  const auto div = original / 2;

  EXPECT_NEAR(div.X().to<double>(), 1.5, kEpsilon);
  EXPECT_NEAR(div.Y().to<double>(), 2.5, kEpsilon);
}

TEST(Translation2dTest, Norm) {
  const Translation2d one{3.0_m, 5.0_m};
  EXPECT_NEAR(one.Norm().to<double>(), std::hypot(3, 5), kEpsilon);
}

TEST(Translation2dTest, Distance) {
  const Translation2d one{1_m, 1_m};
  const Translation2d two{6_m, 6_m};
  EXPECT_NEAR(one.Distance(two).to<double>(), 5 * std::sqrt(2), kEpsilon);
}

TEST(Translation2dTest, UnaryMinus) {
  const Translation2d original{-4.5_m, 7_m};
  const auto inverted = -original;

  EXPECT_NEAR(inverted.X().to<double>(), 4.5, kEpsilon);
  EXPECT_NEAR(inverted.Y().to<double>(), -7, kEpsilon);
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
  EXPECT_NEAR(one.X().to<double>(), 1.0, kEpsilon);
  EXPECT_NEAR(one.Y().to<double>(), 1.0, kEpsilon);

  Translation2d two{2_m, Rotation2d(60_deg)};
  EXPECT_NEAR(two.X().to<double>(), 1.0, kEpsilon);
  EXPECT_NEAR(two.Y().to<double>(), std::sqrt(3.0), kEpsilon);
}

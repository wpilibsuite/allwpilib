/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "frc/geometry/Translation2d.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(Translation2dTest, SumAndDifference) {
  const Translation2d one{1.0, 3.0};
  const Translation2d two{2.0, 5.0};

  const auto sum = one + two;
  const auto difference = one - two;

  EXPECT_NEAR(sum.X(), 3.0, kEpsilon);
  EXPECT_NEAR(sum.Y(), 8.0, kEpsilon);
  EXPECT_NEAR(difference.X(), -1.0, kEpsilon);
  EXPECT_NEAR(difference.Y(), -2.0, kEpsilon);
}

TEST(Translation2dTest, RotateBy) {
  const Translation2d another{3.0, 0.0};
  const auto rotated = another.RotateBy(Rotation2d::FromDegrees(90.0));

  EXPECT_NEAR(rotated.X(), 0.0, kEpsilon);
  EXPECT_NEAR(rotated.Y(), 3.0, kEpsilon);
}

TEST(Translation2dTest, Scaling) {
  const Translation2d original{3.0, 5.0};
  const auto mult = original * 3;
  const auto div = original / 2;

  EXPECT_NEAR(mult.X(), 9.0, kEpsilon);
  EXPECT_NEAR(mult.Y(), 15.0, kEpsilon);
  EXPECT_NEAR(div.X(), 1.5, kEpsilon);
  EXPECT_NEAR(div.Y(), 2.5, kEpsilon);
}

TEST(Translation2dTest, Norm) {
  const Translation2d one{3.0, 5.0};
  EXPECT_NEAR(one.Norm(), std::hypot(3, 5), kEpsilon);
}

TEST(Translation2dTest, Distance) {
  const Translation2d one{1, 1};
  const Translation2d two{6, 6};
  EXPECT_NEAR(one.Distance(two), 5 * std::sqrt(2), kEpsilon);
}

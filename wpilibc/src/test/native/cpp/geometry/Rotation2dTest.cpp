/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "frc/geometry/Rotation2d.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;
static constexpr double kPi = 3.14159265358979323846;

TEST(Rotation2dTest, RadiansToDegrees) {
  const Rotation2d one{kPi / 3};
  const Rotation2d two{kPi / 4};

  EXPECT_NEAR(one.Degrees(), 60.0, kEpsilon);
  EXPECT_NEAR(two.Degrees(), 45.0, kEpsilon);
}

TEST(Rotation2dTest, DegreesToRadians) {
  const auto one = Rotation2d::FromDegrees(45.0);
  const auto two = Rotation2d::FromDegrees(30.0);

  EXPECT_NEAR(one.Radians(), kPi / 4.0, kEpsilon);
  EXPECT_NEAR(two.Radians(), kPi / 6.0, kEpsilon);
}

TEST(Rotation2dTest, RotateByFromZero) {
  const Rotation2d zero;
  auto sum = zero + Rotation2d::FromDegrees(90.0);

  EXPECT_NEAR(sum.Radians(), kPi / 2.0, kEpsilon);
  EXPECT_NEAR(sum.Degrees(), 90.0, kEpsilon);
}

TEST(Rotation2dTest, RotateByNonZero) {
  auto rot = Rotation2d::FromDegrees(90.0);
  rot += Rotation2d::FromDegrees(30.0);

  EXPECT_NEAR(rot.Degrees(), 120.0, kEpsilon);
}

TEST(Rotation2dTest, Minus) {
  const auto one = Rotation2d::FromDegrees(70.0);
  const auto two = Rotation2d::FromDegrees(30.0);

  EXPECT_NEAR((one - two).Degrees(), 40.0, kEpsilon);
}
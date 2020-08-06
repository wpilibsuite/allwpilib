/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include <wpi/math>

#include "frc/geometry/Rotation2d.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(Rotation2dTest, RadiansToDegrees) {
  const Rotation2d one{units::radian_t(wpi::math::pi / 3)};
  const Rotation2d two{units::radian_t(wpi::math::pi / 4)};

  EXPECT_NEAR(one.Degrees().to<double>(), 60.0, kEpsilon);
  EXPECT_NEAR(two.Degrees().to<double>(), 45.0, kEpsilon);
}

TEST(Rotation2dTest, DegreesToRadians) {
  const auto one = Rotation2d(45.0_deg);
  const auto two = Rotation2d(30.0_deg);

  EXPECT_NEAR(one.Radians().to<double>(), wpi::math::pi / 4.0, kEpsilon);
  EXPECT_NEAR(two.Radians().to<double>(), wpi::math::pi / 6.0, kEpsilon);
}

TEST(Rotation2dTest, RotateByFromZero) {
  const Rotation2d zero;
  auto sum = zero + Rotation2d(90.0_deg);

  EXPECT_NEAR(sum.Radians().to<double>(), wpi::math::pi / 2.0, kEpsilon);
  EXPECT_NEAR(sum.Degrees().to<double>(), 90.0, kEpsilon);
}

TEST(Rotation2dTest, RotateByNonZero) {
  auto rot = Rotation2d(90.0_deg);
  rot += Rotation2d(30.0_deg);

  EXPECT_NEAR(rot.Degrees().to<double>(), 120.0, kEpsilon);
}

TEST(Rotation2dTest, Minus) {
  const auto one = Rotation2d(70.0_deg);
  const auto two = Rotation2d(30.0_deg);

  EXPECT_NEAR((one - two).Degrees().to<double>(), 40.0, kEpsilon);
}

TEST(Rotation2dTest, Equality) {
  const auto one = Rotation2d(43_deg);
  const auto two = Rotation2d(43_deg);
  EXPECT_TRUE(one == two);
}

TEST(Rotation2dTest, Inequality) {
  const auto one = Rotation2d(43_deg);
  const auto two = Rotation2d(43.5_deg);
  EXPECT_TRUE(one != two);
}

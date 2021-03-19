// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <wpi/math>

#include "frc/geometry/Rotation2d.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(Rotation2dTest, RadiansToDegrees) {
  const Rotation2d rot1{units::radian_t(wpi::math::pi / 3)};
  const Rotation2d rot2{units::radian_t(wpi::math::pi / 4)};

  EXPECT_NEAR(rot1.Degrees().to<double>(), 60.0, kEpsilon);
  EXPECT_NEAR(rot2.Degrees().to<double>(), 45.0, kEpsilon);
}

TEST(Rotation2dTest, DegreesToRadians) {
  const auto rot1 = Rotation2d(45.0_deg);
  const auto rot2 = Rotation2d(30.0_deg);

  EXPECT_NEAR(rot1.Radians().to<double>(), wpi::math::pi / 4.0, kEpsilon);
  EXPECT_NEAR(rot2.Radians().to<double>(), wpi::math::pi / 6.0, kEpsilon);
}

TEST(Rotation2dTest, RotateByFromZero) {
  const Rotation2d zero;
  auto sum = zero + Rotation2d(90.0_deg);

  EXPECT_NEAR(sum.Radians().to<double>(), wpi::math::pi / 2.0, kEpsilon);
  EXPECT_NEAR(sum.Degrees().to<double>(), 90.0, kEpsilon);
}

TEST(Rotation2dTest, RotateByNonZero) {
  auto rot = Rotation2d(90.0_deg);
  rot = rot + Rotation2d(30.0_deg);

  EXPECT_NEAR(rot.Degrees().to<double>(), 120.0, kEpsilon);
}

TEST(Rotation2dTest, Minus) {
  const auto rot1 = Rotation2d(70.0_deg);
  const auto rot2 = Rotation2d(30.0_deg);

  EXPECT_NEAR((rot1 - rot2).Degrees().to<double>(), 40.0, kEpsilon);
}

TEST(Rotation2dTest, Equality) {
  const auto rot1 = Rotation2d(43_deg);
  const auto rot2 = Rotation2d(43_deg);
  EXPECT_EQ(rot1, rot2);

  const auto rot3 = Rotation2d(-180_deg);
  const auto rot4 = Rotation2d(180_deg);
  EXPECT_EQ(rot3, rot4);
}

TEST(Rotation2dTest, Inequality) {
  const auto rot1 = Rotation2d(43_deg);
  const auto rot2 = Rotation2d(43.5_deg);
  EXPECT_NE(rot1, rot2);
}

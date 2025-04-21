// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>

#include <gtest/gtest.h>

#include "frc/geometry/Rotation2d.h"

using namespace frc;

TEST(Rotation2dTest, RadiansToDegrees) {
  const Rotation2d rot1{units::radian_t{std::numbers::pi / 3.0}};
  const Rotation2d rot2{units::radian_t{std::numbers::pi / 4.0}};

  EXPECT_DOUBLE_EQ(60.0, rot1.Degrees().value());
  EXPECT_DOUBLE_EQ(45.0, rot2.Degrees().value());
}

TEST(Rotation2dTest, DegreesToRadians) {
  const auto rot1 = Rotation2d{45_deg};
  const auto rot2 = Rotation2d{30_deg};

  EXPECT_DOUBLE_EQ(std::numbers::pi / 4.0, rot1.Radians().value());
  EXPECT_DOUBLE_EQ(std::numbers::pi / 6.0, rot2.Radians().value());
}

TEST(Rotation2dTest, RotateByFromZero) {
  const Rotation2d zero;
  auto rotated = zero + Rotation2d{90_deg};

  EXPECT_DOUBLE_EQ(std::numbers::pi / 2.0, rotated.Radians().value());
  EXPECT_DOUBLE_EQ(90.0, rotated.Degrees().value());
}

TEST(Rotation2dTest, RotateByNonZero) {
  auto rot = Rotation2d{90_deg};
  rot = rot + Rotation2d{30_deg};

  EXPECT_DOUBLE_EQ(120.0, rot.Degrees().value());
}

TEST(Rotation2dTest, Minus) {
  const auto rot1 = Rotation2d{70_deg};
  const auto rot2 = Rotation2d{30_deg};

  EXPECT_DOUBLE_EQ(40.0, (rot1 - rot2).Degrees().value());
}

TEST(Rotation2dTest, UnaryMinus) {
  const auto rot = Rotation2d{20_deg};

  EXPECT_DOUBLE_EQ(-20.0, (-rot).Degrees().value());
}

TEST(Rotation2dTest, Multiply) {
  const auto rot = Rotation2d{10_deg};

  EXPECT_DOUBLE_EQ(30.0, (rot * 3.0).Degrees().value());
  EXPECT_DOUBLE_EQ(410.0, (rot * 41.0).Degrees().value());
}

TEST(Rotation2dTest, Equality) {
  auto rot1 = Rotation2d{43_deg};
  auto rot2 = Rotation2d{43_deg};
  EXPECT_EQ(rot1, rot2);

  rot1 = Rotation2d{-180_deg};
  rot2 = Rotation2d{180_deg};
  EXPECT_EQ(rot1, rot2);
}

TEST(Rotation2dTest, Inequality) {
  const auto rot1 = Rotation2d{43_deg};
  const auto rot2 = Rotation2d{43.5_deg};
  EXPECT_NE(rot1, rot2);
}

TEST(Rotation2dTest, ToMatrix) {
#if __GNUC__ <= 11
  Rotation2d before{20_deg};
  Rotation2d after{before.ToMatrix()};
#else
  constexpr Rotation2d before{20_deg};
  constexpr Rotation2d after{before.ToMatrix()};
#endif

  EXPECT_EQ(before, after);
}

TEST(Rotation2dTest, Constexpr) {
  constexpr Rotation2d defaultCtor;
  constexpr Rotation2d radianCtor{5_rad};
  constexpr Rotation2d degreeCtor{270_deg};
  constexpr Rotation2d rotation45{45_deg};
  constexpr Rotation2d cartesianCtor{3.5, -3.5};

  constexpr auto negated = -radianCtor;
  constexpr auto multiplied = radianCtor * 2;
  constexpr auto subtracted = cartesianCtor - degreeCtor;

  static_assert(defaultCtor.Radians() == 0_rad);
  static_assert(degreeCtor.Degrees() == 270_deg);
  static_assert(negated.Radians() == -5_rad);
  static_assert(multiplied.Radians() == 10_rad);
  static_assert(subtracted == rotation45);
  static_assert(radianCtor != degreeCtor);
}

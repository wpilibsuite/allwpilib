// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <limits>

#include "frc/MathUtil.h"
#include "gtest/gtest.h"
#include "units/angle.h"

#define EXPECT_UNITS_EQ(a, b) EXPECT_DOUBLE_EQ((a).value(), (b).value())

#define EXPECT_UNITS_NEAR(a, b, c) EXPECT_NEAR((a).value(), (b).value(), c)

TEST(MathUtilTest, ApplyDeadbandUnityScale) {
  // < 0
  EXPECT_DOUBLE_EQ(-1.0, frc::ApplyDeadband(-1.0, 0.02));
  EXPECT_DOUBLE_EQ((-0.03 + 0.02) / (1.0 - 0.02),
                   frc::ApplyDeadband(-0.03, 0.02));
  EXPECT_DOUBLE_EQ(0.0, frc::ApplyDeadband(-0.02, 0.02));
  EXPECT_DOUBLE_EQ(0.0, frc::ApplyDeadband(-0.01, 0.02));

  // == 0
  EXPECT_DOUBLE_EQ(0.0, frc::ApplyDeadband(0.0, 0.02));

  // > 0
  EXPECT_DOUBLE_EQ(0.0, frc::ApplyDeadband(0.01, 0.02));
  EXPECT_DOUBLE_EQ(0.0, frc::ApplyDeadband(0.02, 0.02));
  EXPECT_DOUBLE_EQ((0.03 - 0.02) / (1.0 - 0.02),
                   frc::ApplyDeadband(0.03, 0.02));
  EXPECT_DOUBLE_EQ(1.0, frc::ApplyDeadband(1.0, 0.02));
}

TEST(MathUtilTest, ApplyDeadbandArbitraryScale) {
  // < 0
  EXPECT_DOUBLE_EQ(-2.5, frc::ApplyDeadband(-2.5, 0.02, 2.5));
  EXPECT_DOUBLE_EQ(0.0, frc::ApplyDeadband(-0.02, 0.02, 2.5));
  EXPECT_DOUBLE_EQ(0.0, frc::ApplyDeadband(-0.01, 0.02, 2.5));

  // == 0
  EXPECT_DOUBLE_EQ(0.0, frc::ApplyDeadband(0.0, 0.02, 2.5));

  // > 0
  EXPECT_DOUBLE_EQ(0.0, frc::ApplyDeadband(0.01, 0.02, 2.5));
  EXPECT_DOUBLE_EQ(0.0, frc::ApplyDeadband(0.02, 0.02, 2.5));
  EXPECT_DOUBLE_EQ(2.5, frc::ApplyDeadband(2.5, 0.02, 2.5));
}

TEST(MathUtilTest, ApplyDeadbandUnits) {
  // < 0
  EXPECT_DOUBLE_EQ(
      -20, frc::ApplyDeadband<units::radian_t>(-20_rad, 1_rad, 20_rad).value());
}

TEST(MathUtilTest, ApplyDeadbandLargeMaxMagnitude) {
  EXPECT_DOUBLE_EQ(
      80.0,
      frc::ApplyDeadband(100.0, 20.0, std::numeric_limits<double>::infinity()));
}

TEST(MathUtilTest, InputModulus) {
  // These tests check error wrapping. That is, the result of wrapping the
  // result of an angle reference minus the measurement.

  // Test symmetric range
  EXPECT_DOUBLE_EQ(-20.0, frc::InputModulus(170.0 - (-170.0), -180.0, 180.0));
  EXPECT_DOUBLE_EQ(-20.0,
                   frc::InputModulus(170.0 + 360.0 - (-170.0), -180.0, 180.0));
  EXPECT_DOUBLE_EQ(-20.0,
                   frc::InputModulus(170.0 - (-170.0 + 360.0), -180.0, 180.0));
  EXPECT_DOUBLE_EQ(20.0, frc::InputModulus(-170.0 - 170.0, -180.0, 180.0));
  EXPECT_DOUBLE_EQ(20.0,
                   frc::InputModulus(-170.0 + 360.0 - 170.0, -180.0, 180.0));
  EXPECT_DOUBLE_EQ(20.0,
                   frc::InputModulus(-170.0 - (170.0 + 360.0), -180.0, 180.0));

  // Test range starting at zero
  EXPECT_DOUBLE_EQ(340.0, frc::InputModulus(170.0 - 190.0, 0.0, 360.0));
  EXPECT_DOUBLE_EQ(340.0, frc::InputModulus(170.0 + 360.0 - 190.0, 0.0, 360.0));
  EXPECT_DOUBLE_EQ(340.0,
                   frc::InputModulus(170.0 - (190.0 + 360.0), 0.0, 360.0));

  // Test asymmetric range that doesn't start at zero
  EXPECT_DOUBLE_EQ(-20.0, frc::InputModulus(170.0 - (-170.0), -170.0, 190.0));

  // Test range with both positive endpoints
  EXPECT_DOUBLE_EQ(2.0, frc::InputModulus(0.0, 1.0, 3.0));
  EXPECT_DOUBLE_EQ(3.0, frc::InputModulus(1.0, 1.0, 3.0));
  EXPECT_DOUBLE_EQ(2.0, frc::InputModulus(2.0, 1.0, 3.0));
  EXPECT_DOUBLE_EQ(3.0, frc::InputModulus(3.0, 1.0, 3.0));
  EXPECT_DOUBLE_EQ(2.0, frc::InputModulus(4.0, 1.0, 3.0));

  // Test all supported types
  EXPECT_DOUBLE_EQ(-20.0,
                   frc::InputModulus<double>(170.0 - (-170.0), -170.0, 190.0));
  EXPECT_EQ(-20, frc::InputModulus<int>(170 - (-170), -170, 190));
  EXPECT_EQ(-20_deg, frc::InputModulus<units::degree_t>(170_deg - (-170_deg),
                                                        -170_deg, 190_deg));
}

TEST(MathUtilTest, AngleModulus) {
  EXPECT_UNITS_NEAR(
      frc::AngleModulus(units::radian_t{-2000 * std::numbers::pi / 180}),
      units::radian_t{160 * std::numbers::pi / 180}, 1e-10);
  EXPECT_UNITS_NEAR(
      frc::AngleModulus(units::radian_t{358 * std::numbers::pi / 180}),
      units::radian_t{-2 * std::numbers::pi / 180}, 1e-10);
  EXPECT_UNITS_NEAR(frc::AngleModulus(units::radian_t{2.0 * std::numbers::pi}),
                    0_rad, 1e-10);

  EXPECT_UNITS_EQ(frc::AngleModulus(units::radian_t{5 * std::numbers::pi}),
                  units::radian_t{std::numbers::pi});
  EXPECT_UNITS_EQ(frc::AngleModulus(units::radian_t{-5 * std::numbers::pi}),
                  units::radian_t{std::numbers::pi});
  EXPECT_UNITS_EQ(frc::AngleModulus(units::radian_t{std::numbers::pi / 2}),
                  units::radian_t{std::numbers::pi / 2});
  EXPECT_UNITS_EQ(frc::AngleModulus(units::radian_t{-std::numbers::pi / 2}),
                  units::radian_t{-std::numbers::pi / 2});
}

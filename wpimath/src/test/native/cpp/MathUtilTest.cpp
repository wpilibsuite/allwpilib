// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/MathUtil.h"
#include "gtest/gtest.h"
#include "units/angle.h"

#define EXPECT_UNITS_EQ(a, b) \
  EXPECT_FLOAT_EQ((a).to<double>(), (b).to<double>())

TEST(MathUtilTest, InputModulus) {
  // These tests check error wrapping. That is, the result of wrapping the
  // result of an angle reference minus the measurement.

  // Test symmetric range
  EXPECT_FLOAT_EQ(-20.0, frc::InputModulus(170.0 - (-170.0), -180.0, 180.0));
  EXPECT_FLOAT_EQ(-20.0,
                  frc::InputModulus(170.0 + 360.0 - (-170.0), -180.0, 180.0));
  EXPECT_FLOAT_EQ(-20.0,
                  frc::InputModulus(170.0 - (-170.0 + 360.0), -180.0, 180.0));
  EXPECT_FLOAT_EQ(20.0, frc::InputModulus(-170.0 - 170.0, -180.0, 180.0));
  EXPECT_FLOAT_EQ(20.0,
                  frc::InputModulus(-170.0 + 360.0 - 170.0, -180.0, 180.0));
  EXPECT_FLOAT_EQ(20.0,
                  frc::InputModulus(-170.0 - (170.0 + 360.0), -180.0, 180.0));

  // Test range starting at zero
  EXPECT_FLOAT_EQ(340.0, frc::InputModulus(170.0 - 190.0, 0.0, 360.0));
  EXPECT_FLOAT_EQ(340.0, frc::InputModulus(170.0 + 360.0 - 190.0, 0.0, 360.0));
  EXPECT_FLOAT_EQ(340.0,
                  frc::InputModulus(170.0 - (190.0 + 360.0), 0.0, 360.0));

  // Test asymmetric range that doesn't start at zero
  EXPECT_FLOAT_EQ(-20.0, frc::InputModulus(170.0 - (-170.0), -170.0, 190.0));

  // Test range with both positive endpoints
  EXPECT_FLOAT_EQ(2.0, frc::InputModulus(0.0, 1.0, 3.0));
  EXPECT_FLOAT_EQ(3.0, frc::InputModulus(1.0, 1.0, 3.0));
  EXPECT_FLOAT_EQ(2.0, frc::InputModulus(2.0, 1.0, 3.0));
  EXPECT_FLOAT_EQ(3.0, frc::InputModulus(3.0, 1.0, 3.0));
  EXPECT_FLOAT_EQ(2.0, frc::InputModulus(4.0, 1.0, 3.0));

  // Test all supported types
  EXPECT_FLOAT_EQ(-20.0,
                  frc::InputModulus<double>(170.0 - (-170.0), -170.0, 190.0));
  EXPECT_EQ(-20, frc::InputModulus<int>(170 - (-170), -170, 190));
  EXPECT_EQ(-20_deg, frc::InputModulus<units::degree_t>(170_deg - (-170_deg),
                                                        -170_deg, 190_deg));
}

TEST(MathUtilTest, AngleModulus) {
  EXPECT_UNITS_EQ(
      frc::AngleModulus(units::radian_t{-2000 * wpi::numbers::pi / 180}),
      units::radian_t{160 * wpi::numbers::pi / 180});
  EXPECT_UNITS_EQ(
      frc::AngleModulus(units::radian_t{358 * wpi::numbers::pi / 180}),
      units::radian_t{-2 * wpi::numbers::pi / 180});
  EXPECT_UNITS_EQ(frc::AngleModulus(units::radian_t{2.0 * wpi::numbers::pi}),
                  0_rad);

  EXPECT_UNITS_EQ(frc::AngleModulus(units::radian_t(5 * wpi::numbers::pi)),
                  units::radian_t(wpi::numbers::pi));
  EXPECT_UNITS_EQ(frc::AngleModulus(units::radian_t(-5 * wpi::numbers::pi)),
                  units::radian_t(wpi::numbers::pi));
  EXPECT_UNITS_EQ(frc::AngleModulus(units::radian_t(wpi::numbers::pi / 2)),
                  units::radian_t(wpi::numbers::pi / 2));
  EXPECT_UNITS_EQ(frc::AngleModulus(units::radian_t(-wpi::numbers::pi / 2)),
                  units::radian_t(-wpi::numbers::pi / 2));
}

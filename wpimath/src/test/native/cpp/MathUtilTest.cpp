// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <limits>

#include <gtest/gtest.h>

#include "frc/MathUtil.h"
#include "frc/geometry/Translation3d.h"
#include "frc/geometry/Translation2d.h"
#include "units/time.h"
#include "units/velocity.h"
#include "units/length.h"
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

TEST(MathUtilTest, IsNear) {
  // The answer is always 42
  // Positive integer checks
  EXPECT_TRUE(frc::IsNear(42, 42, 1));
  EXPECT_TRUE(frc::IsNear(42, 41, 2));
  EXPECT_TRUE(frc::IsNear(42, 43, 2));
  EXPECT_FALSE(frc::IsNear(42, 44, 1));

  // Negative integer checks
  EXPECT_TRUE(frc::IsNear(-42, -42, 1));
  EXPECT_TRUE(frc::IsNear(-42, -41, 2));
  EXPECT_TRUE(frc::IsNear(-42, -43, 2));
  EXPECT_FALSE(frc::IsNear(-42, -44, 1));

  // Mixed sign integer checks
  EXPECT_FALSE(frc::IsNear(-42, 42, 1));
  EXPECT_FALSE(frc::IsNear(-42, 41, 2));
  EXPECT_FALSE(frc::IsNear(-42, 43, 2));
  EXPECT_FALSE(frc::IsNear(42, -42, 1));
  EXPECT_FALSE(frc::IsNear(42, -41, 2));
  EXPECT_FALSE(frc::IsNear(42, -43, 2));

  // Floating point checks
  EXPECT_TRUE(frc::IsNear<double>(42, 41.5, 1));
  EXPECT_TRUE(frc::IsNear<double>(42, 42.5, 1));
  EXPECT_TRUE(frc::IsNear<double>(42, 41.5, 0.75));
  EXPECT_TRUE(frc::IsNear<double>(42, 42.5, 0.75));

  // Wraparound checks
  EXPECT_TRUE(frc::IsNear(0_deg, 356_deg, 5_deg, 0_deg, 360_deg));
  EXPECT_TRUE(frc::IsNear(0, -356, 5, 0, 360));
  EXPECT_TRUE(frc::IsNear(0, 4, 5, 0, 360));
  EXPECT_TRUE(frc::IsNear(0, -4, 5, 0, 360));
  EXPECT_TRUE(frc::IsNear(400, 41, 5, 0, 360));
  EXPECT_TRUE(frc::IsNear(400, -319, 5, 0, 360));
  EXPECT_TRUE(frc::IsNear(400, 401, 5, 0, 360));
  EXPECT_FALSE(frc::IsNear<double>(0, 356, 2.5, 0, 360));
  EXPECT_FALSE(frc::IsNear<double>(0, -356, 2.5, 0, 360));
  EXPECT_FALSE(frc::IsNear<double>(0, 4, 2.5, 0, 360));
  EXPECT_FALSE(frc::IsNear<double>(0, -4, 2.5, 0, 360));
  EXPECT_FALSE(frc::IsNear(400, 35, 5, 0, 360));
  EXPECT_FALSE(frc::IsNear(400, -315, 5, 0, 360));
  EXPECT_FALSE(frc::IsNear(400, 395, 5, 0, 360));
  EXPECT_FALSE(frc::IsNear(0_deg, -4_deg, 2.5_deg, 0_deg, 360_deg));
}

TEST(MathUtilTest, Translation2dSlewRateLimit) {
  const frc::Translation2d translation1{0_m, 0_m};
  const frc::Translation2d translation2{2_m, 2_m};
  const frc::Translation2d translation3{1_m, 1_m};
  const frc::Translation2d translation4{3_m, 3_m};
  const frc::Translation2d result1 = frc::SlewRateLimit(translation1, translation2, 1_s, 50_mps);
  const frc::Translation2d result2 = frc::SlewRateLimit(translation3, translation4, 0.25_s, 2_mps);
  const frc::Translation2d expected1{2_m, 2_m};
  const frc::Translation2d expected2{units::meter_t{1.0 + 0.5 * std::numbers::inv_sqrt2}, 1.0 + 0.5 * std::numbers::inv_sqrt2}, 1.0 + 0.5 * std::numbers::inv_sqrt2}};
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
}

TEST(MathUtilTest, Translation3dSlewRateLimit) {
  const frc::Translation3d translation1{0_m, 0_m, 0_m};
  const frc::Translation3d translation2{2_m, 2_m, 2_m};
  const frc::Translation3d translation3{1_m, 1_m, 1_m};
  const frc::Translation3d translation4{3_m, 3_m, 3_m};
  const frc::Translation3d result1 = frc::SlewRateLimit(translation1, translation2, 1_s, 50.0_mps);
  const frc::Translation3d result2 = frc::SlewRateLimit(translation3, translation4, 0.25_s, 2.0_mps);
  const frc::Translation3d expected1{2_m, 2_m, 2_m};
  const frc::Translation3d expected2{units::meter_t{1.0 + 0.5 * std::numbers::inv_sqrt3}, units::meter_t{1.0 + 0.5 * std::numbers::inv_sqrt3}, units::meter_t{1.0 + 0.5 * std::numbers::inv_sqrt3}};
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
}
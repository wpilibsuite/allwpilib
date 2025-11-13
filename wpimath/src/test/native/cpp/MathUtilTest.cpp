// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/util/MathUtil.hpp"

#include <limits>
#include <numbers>

#include <gtest/gtest.h>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

#define EXPECT_UNITS_EQ(a, b) EXPECT_DOUBLE_EQ((a).value(), (b).value())

#define EXPECT_UNITS_NEAR(a, b, c) EXPECT_NEAR((a).value(), (b).value(), c)

TEST(MathUtilTest, ApplyDeadbandUnityScale) {
  // < 0
  EXPECT_DOUBLE_EQ(-1.0, wpi::math::ApplyDeadband(-1.0, 0.02));
  EXPECT_DOUBLE_EQ((-0.03 + 0.02) / (1.0 - 0.02),
                   wpi::math::ApplyDeadband(-0.03, 0.02));
  EXPECT_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(-0.02, 0.02));
  EXPECT_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(-0.01, 0.02));

  // == 0
  EXPECT_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.0, 0.02));

  // > 0
  EXPECT_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.01, 0.02));
  EXPECT_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.02, 0.02));
  EXPECT_DOUBLE_EQ((0.03 - 0.02) / (1.0 - 0.02),
                   wpi::math::ApplyDeadband(0.03, 0.02));
  EXPECT_DOUBLE_EQ(1.0, wpi::math::ApplyDeadband(1.0, 0.02));
}

TEST(MathUtilTest, ApplyDeadbandArbitraryScale) {
  // < 0
  EXPECT_DOUBLE_EQ(-2.5, wpi::math::ApplyDeadband(-2.5, 0.02, 2.5));
  EXPECT_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(-0.02, 0.02, 2.5));
  EXPECT_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(-0.01, 0.02, 2.5));

  // == 0
  EXPECT_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.0, 0.02, 2.5));

  // > 0
  EXPECT_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.01, 0.02, 2.5));
  EXPECT_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.02, 0.02, 2.5));
  EXPECT_DOUBLE_EQ(2.5, wpi::math::ApplyDeadband(2.5, 0.02, 2.5));
}

TEST(MathUtilTest, ApplyDeadbandUnits) {
  // < 0
  EXPECT_UNITS_EQ(-20_rad, wpi::math::ApplyDeadband<wpi::units::radian_t>(
                               -20_rad, 1_rad, 20_rad));
}

TEST(MathUtilTest, ApplyDeadbandLargeMaxMagnitude) {
  EXPECT_DOUBLE_EQ(
      80.0, wpi::math::ApplyDeadband(100.0, 20.0,
                                     std::numeric_limits<double>::infinity()));
}

TEST(MathUtilTest, ApplyDeadband2dUnityScale) {
  EXPECT_EQ((Eigen::Vector2d{{0.0}, {1.0}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {1.0}}, 0.02));
  EXPECT_EQ((Eigen::Vector2d{{0.0}, {-1.0}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {-1.0}}, 0.02));
  EXPECT_EQ((Eigen::Vector2d{{-1.0}, {0.0}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{-1.0}, {0.0}}, 0.02));

  // == 0
  EXPECT_EQ((Eigen::Vector2d{{0.0}, {0.0}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {0.0}}, 0.02));

  // > 0
  EXPECT_EQ((Eigen::Vector2d{{0.0}, {0.0}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{0.01}, {0.0}}, 0.02));
  EXPECT_EQ((Eigen::Vector2d{{0.0}, {0.0}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{0.02}, {0.0}}, 0.02));
  EXPECT_EQ((Eigen::Vector2d{{(0.03 - 0.02) / (1.0 - 0.02)}, {0.0}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{0.03}, {0.0}}, 0.02));
  EXPECT_EQ((Eigen::Vector2d{{1.0}, {0.0}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{1.0}, {0.0}}, 0.02));
}

TEST(MathUtilTest, ApplyDeadband2dArbitraryScale) {
  EXPECT_EQ((Eigen::Vector2d{{0.0}, {2.5}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {2.5}}, 0.02, 2.5));
  EXPECT_EQ(
      (Eigen::Vector2d{{0.0}, {-2.5}}),
      wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {-2.5}}, 0.02, 2.5));
  EXPECT_EQ(
      (Eigen::Vector2d{{-2.5}, {0.0}}),
      wpi::math::ApplyDeadband(Eigen::Vector2d{{-2.5}, {0.0}}, 0.02, 2.5));

  // == 0
  EXPECT_EQ((Eigen::Vector2d{{0.0}, {0.0}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {0.0}}, 0.02, 2.5));

  // > 0
  EXPECT_EQ(
      (Eigen::Vector2d{{0.0}, {0.0}}),
      wpi::math::ApplyDeadband(Eigen::Vector2d{{0.01}, {0.0}}, 0.02, 2.5));
  EXPECT_EQ(
      (Eigen::Vector2d{{0.0}, {0.0}}),
      wpi::math::ApplyDeadband(Eigen::Vector2d{{0.02}, {0.0}}, 0.02, 2.5));
  EXPECT_EQ((Eigen::Vector2d{{2.5}, {0.0}}),
            wpi::math::ApplyDeadband(Eigen::Vector2d{{2.5}, {0.0}}, 0.02, 2.5));
}

TEST(MathUtilTest, ApplyDeadband2dLargeMaxMagnitude) {
  EXPECT_EQ(
      (Eigen::Vector2d{{80.0}, {0.0}}),
      (wpi::math::ApplyDeadband(Eigen::Vector2d{{100.0}, {0.0}}, 20.0,
                                std::numeric_limits<double>::infinity())));
}

TEST(MathUtilTest, ApplyDeadband2dUnits) {
  EXPECT_EQ(
      (Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 2.5_mps}),
      wpi::math::ApplyDeadband(
          Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 2.5_mps},
          0.02_mps, 2.5_mps));
  EXPECT_EQ((Eigen::Vector<wpi::units::meters_per_second_t, 2>{1_mps, 0_mps}),
            wpi::math::ApplyDeadband(
                Eigen::Vector<wpi::units::meters_per_second_t, 2>{1_mps, 0_mps},
                0.02_mps));

  EXPECT_EQ((Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 0_mps}),
            wpi::math::ApplyDeadband(
                Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 0_mps},
                0.02_mps, 2.5_mps));
}

TEST(MathUtilTest, CopyDirectionPow) {
  EXPECT_DOUBLE_EQ(0.5, wpi::math::CopyDirectionPow(0.5, 1.0));
  EXPECT_DOUBLE_EQ(-0.5, wpi::math::CopyDirectionPow(-0.5, 1.0));

  EXPECT_DOUBLE_EQ(0.5 * 0.5, wpi::math::CopyDirectionPow(0.5, 2.0));
  EXPECT_DOUBLE_EQ(-(0.5 * 0.5), wpi::math::CopyDirectionPow(-0.5, 2.0));

  EXPECT_DOUBLE_EQ(std::sqrt(0.5), wpi::math::CopyDirectionPow(0.5, 0.5));
  EXPECT_DOUBLE_EQ(-std::sqrt(0.5), wpi::math::CopyDirectionPow(-0.5, 0.5));

  EXPECT_DOUBLE_EQ(0.0, wpi::math::CopyDirectionPow(0.0, 2.0));
  EXPECT_DOUBLE_EQ(1.0, wpi::math::CopyDirectionPow(1.0, 2.0));
  EXPECT_DOUBLE_EQ(-1.0, wpi::math::CopyDirectionPow(-1.0, 2.0));

  EXPECT_DOUBLE_EQ(std::pow(0.8, 0.3), wpi::math::CopyDirectionPow(0.8, 0.3));
  EXPECT_DOUBLE_EQ(-std::pow(0.8, 0.3), wpi::math::CopyDirectionPow(-0.8, 0.3));
}

TEST(MathUtilTest, CopyDirectionPowWithMaxMagnitude) {
  EXPECT_DOUBLE_EQ(5.0, wpi::math::CopyDirectionPow(5.0, 1.0, 10.0));
  EXPECT_DOUBLE_EQ(-5.0, wpi::math::CopyDirectionPow(-5.0, 1.0, 10.0));

  EXPECT_DOUBLE_EQ(0.5 * 0.5 * 10, wpi::math::CopyDirectionPow(5.0, 2.0, 10.0));
  EXPECT_DOUBLE_EQ(-0.5 * 0.5 * 10,
                   wpi::math::CopyDirectionPow(-5.0, 2.0, 10.0));

  EXPECT_DOUBLE_EQ(std::sqrt(0.5) * 10,
                   wpi::math::CopyDirectionPow(5.0, 0.5, 10.0));
  EXPECT_DOUBLE_EQ(-std::sqrt(0.5) * 10,
                   wpi::math::CopyDirectionPow(-5.0, 0.5, 10.0));

  EXPECT_DOUBLE_EQ(0.0, wpi::math::CopyDirectionPow(0.0, 2.0, 5.0));
  EXPECT_DOUBLE_EQ(5.0, wpi::math::CopyDirectionPow(5.0, 2.0, 5.0));
  EXPECT_DOUBLE_EQ(-5.0, wpi::math::CopyDirectionPow(-5.0, 2.0, 5.0));

  EXPECT_DOUBLE_EQ(std::pow(0.8, 0.3) * 100,
                   wpi::math::CopyDirectionPow(80.0, 0.3, 100.0));
  EXPECT_DOUBLE_EQ(-std::pow(0.8, 0.3) * 100,
                   wpi::math::CopyDirectionPow(-80.0, 0.3, 100.0));
}

TEST(MathUtilTest, CopyDirectionPowWithUnits) {
  EXPECT_UNITS_EQ(
      0_mps,
      wpi::math::CopyDirectionPow<wpi::units::meters_per_second_t>(0_mps, 2.0));
  EXPECT_UNITS_EQ(
      1_mps,
      wpi::math::CopyDirectionPow<wpi::units::meters_per_second_t>(1_mps, 2.0));
  EXPECT_UNITS_EQ(-1_mps,
                  wpi::math::CopyDirectionPow<wpi::units::meters_per_second_t>(
                      -1_mps, 2.0));

  EXPECT_UNITS_EQ(wpi::units::meters_per_second_t{0.5 * 0.5 * 10},
                  wpi::math::CopyDirectionPow<wpi::units::meters_per_second_t>(
                      5_mps, 2.0, 10_mps));
  EXPECT_UNITS_EQ(wpi::units::meters_per_second_t{-0.5 * 0.5 * 10},
                  wpi::math::CopyDirectionPow<wpi::units::meters_per_second_t>(
                      -5_mps, 2.0, 10_mps));
}

TEST(MathUtilTest, CopyDirectionPow2d) {
  EXPECT_EQ((Eigen::Vector2d{{0.5}, {0.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.5}, {0.0}}, 1.0));
  EXPECT_EQ((Eigen::Vector2d{{-0.5}, {0.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{-0.5}, {0.0}}, 1.0));

  EXPECT_EQ((Eigen::Vector2d{{0.25}, {0.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.5}, {0.0}}, 2.0));
  EXPECT_EQ((Eigen::Vector2d{{-0.25}, {0.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{-0.5}, {0.0}}, 2.0));

  EXPECT_EQ((Eigen::Vector2d{{std::sqrt(0.5)}, {0.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.5}, {0.0}}, 0.5));
  EXPECT_EQ((Eigen::Vector2d{{-std::sqrt(0.5)}, {0.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{-0.5}, {0.0}}, 0.5));

  EXPECT_EQ((Eigen::Vector2d{{0.0}, {0.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {0.0}}, 2.0));

  EXPECT_EQ((Eigen::Vector2d{{1.0}, {0.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{1.0}, {0.0}}, 2.0));
  EXPECT_EQ((Eigen::Vector2d{{-1.0}, {0.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{-1.0}, {0.0}}, 2.0));
  EXPECT_EQ((Eigen::Vector2d{{0.0}, {1.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {1.0}}, 2.0));
  EXPECT_EQ((Eigen::Vector2d{{0.0}, {-1.0}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {-1.0}}, 2.0));

  EXPECT_EQ((Eigen::Vector2d{{0.0}, {std::pow(0.8, 0.3)}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {0.8}}, 0.3));
  EXPECT_EQ((Eigen::Vector2d{{0.0}, {-std::pow(0.8, 0.3)}}),
            wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {-0.8}}, 0.3));
}

TEST(MathUtilTest, CopyDirectionPow2dMaxDistance) {
  EXPECT_EQ(
      (Eigen::Vector2d{{5.0}, {0.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{5.0}, {0.0}}, 1.0, 10.0));
  EXPECT_EQ(
      (Eigen::Vector2d{{-5.0}, {0.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{-5.0}, {0.0}}, 1.0, 10.0));

  EXPECT_EQ(
      (Eigen::Vector2d{{2.5}, {0.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{5.0}, {0.0}}, 2.0, 10.0));
  EXPECT_EQ(
      (Eigen::Vector2d{{-2.5}, {0.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{-5.0}, {0.0}}, 2.0, 10.0));

  EXPECT_EQ(
      (Eigen::Vector2d{{std::sqrt(0.5) * 10.0}, {0.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{5.0}, {0.0}}, 0.5, 10.0));
  EXPECT_EQ(
      (Eigen::Vector2d{{-std::sqrt(0.5) * 10.0}, {0.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{-5.0}, {0.0}}, 0.5, 10.0));

  EXPECT_EQ(
      (Eigen::Vector2d{{0.0}, {0.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {0.0}}, 2.0, 5.0));
  EXPECT_EQ(
      (Eigen::Vector2d{{5.0}, {0.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{5.0}, {0.0}}, 2.0, 5.0));
  EXPECT_EQ(
      (Eigen::Vector2d{{-5.0}, {0.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{-5.0}, {0.0}}, 2.0, 5.0));

  EXPECT_EQ(
      (Eigen::Vector2d{{0.0}, {std::pow(0.8, 0.3) * 100.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {80.0}}, 0.3, 100.0));
  EXPECT_EQ(
      (Eigen::Vector2d{{0.0}, {-std::pow(0.8, 0.3) * 100.0}}),
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {-80.0}}, 0.3, 100.0));
}

TEST(MathUtilTest, CopyDirectionPow2dUnits) {
  EXPECT_EQ((Eigen::Vector<wpi::units::meters_per_second_t, 2>{1_mps, 0_mps}),
            wpi::math::CopyDirectionPow(
                Eigen::Vector<wpi::units::meters_per_second_t, 2>{1_mps, 0_mps},
                2.0));
  EXPECT_EQ(
      (Eigen::Vector<wpi::units::meters_per_second_t, 2>{-1_mps, 0_mps}),
      wpi::math::CopyDirectionPow(
          Eigen::Vector<wpi::units::meters_per_second_t, 2>{-1_mps, 0_mps},
          2.0));

  EXPECT_EQ((Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 0_mps}),
            wpi::math::CopyDirectionPow(
                Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 0_mps},
                2.0, 5_mps));

  EXPECT_EQ((Eigen::Vector<wpi::units::meters_per_second_t, 2>{5_mps, 0_mps}),
            wpi::math::CopyDirectionPow(
                Eigen::Vector<wpi::units::meters_per_second_t, 2>{5_mps, 0_mps},
                2.0, 5_mps));
  EXPECT_EQ(
      (Eigen::Vector<wpi::units::meters_per_second_t, 2>{-5_mps, 0_mps}),
      wpi::math::CopyDirectionPow(
          Eigen::Vector<wpi::units::meters_per_second_t, 2>{-5_mps, 0_mps}, 2.0,
          5_mps));
}

TEST(MathUtilTest, InputModulus) {
  // These tests check error wrapping. That is, the result of wrapping the
  // result of an angle reference minus the measurement.

  // Test symmetric range
  EXPECT_DOUBLE_EQ(-20.0,
                   wpi::math::InputModulus(170.0 - (-170.0), -180.0, 180.0));
  EXPECT_DOUBLE_EQ(
      -20.0, wpi::math::InputModulus(170.0 + 360.0 - (-170.0), -180.0, 180.0));
  EXPECT_DOUBLE_EQ(
      -20.0, wpi::math::InputModulus(170.0 - (-170.0 + 360.0), -180.0, 180.0));
  EXPECT_DOUBLE_EQ(20.0,
                   wpi::math::InputModulus(-170.0 - 170.0, -180.0, 180.0));
  EXPECT_DOUBLE_EQ(
      20.0, wpi::math::InputModulus(-170.0 + 360.0 - 170.0, -180.0, 180.0));
  EXPECT_DOUBLE_EQ(
      20.0, wpi::math::InputModulus(-170.0 - (170.0 + 360.0), -180.0, 180.0));

  // Test range starting at zero
  EXPECT_DOUBLE_EQ(340.0, wpi::math::InputModulus(170.0 - 190.0, 0.0, 360.0));
  EXPECT_DOUBLE_EQ(340.0,
                   wpi::math::InputModulus(170.0 + 360.0 - 190.0, 0.0, 360.0));
  EXPECT_DOUBLE_EQ(
      340.0, wpi::math::InputModulus(170.0 - (190.0 + 360.0), 0.0, 360.0));

  // Test asymmetric range that doesn't start at zero
  EXPECT_DOUBLE_EQ(-20.0,
                   wpi::math::InputModulus(170.0 - (-170.0), -170.0, 190.0));

  // Test range with both positive endpoints
  EXPECT_DOUBLE_EQ(2.0, wpi::math::InputModulus(0.0, 1.0, 3.0));
  EXPECT_DOUBLE_EQ(3.0, wpi::math::InputModulus(1.0, 1.0, 3.0));
  EXPECT_DOUBLE_EQ(2.0, wpi::math::InputModulus(2.0, 1.0, 3.0));
  EXPECT_DOUBLE_EQ(3.0, wpi::math::InputModulus(3.0, 1.0, 3.0));
  EXPECT_DOUBLE_EQ(2.0, wpi::math::InputModulus(4.0, 1.0, 3.0));

  // Test all supported types
  EXPECT_DOUBLE_EQ(
      -20.0, wpi::math::InputModulus<double>(170.0 - (-170.0), -170.0, 190.0));
  EXPECT_EQ(-20, wpi::math::InputModulus<int>(170 - (-170), -170, 190));
  EXPECT_EQ(-20_deg, wpi::math::InputModulus<wpi::units::degree_t>(
                         170_deg - (-170_deg), -170_deg, 190_deg));
}

TEST(MathUtilTest, AngleModulus) {
  EXPECT_UNITS_NEAR(wpi::math::AngleModulus(
                        wpi::units::radian_t{-2000 * std::numbers::pi / 180}),
                    wpi::units::radian_t{160 * std::numbers::pi / 180}, 1e-10);
  EXPECT_UNITS_NEAR(wpi::math::AngleModulus(
                        wpi::units::radian_t{358 * std::numbers::pi / 180}),
                    wpi::units::radian_t{-2 * std::numbers::pi / 180}, 1e-10);
  EXPECT_UNITS_NEAR(
      wpi::math::AngleModulus(wpi::units::radian_t{2.0 * std::numbers::pi}),
      0_rad, 1e-10);

  EXPECT_UNITS_EQ(
      wpi::math::AngleModulus(wpi::units::radian_t{5 * std::numbers::pi}),
      wpi::units::radian_t{std::numbers::pi});
  EXPECT_UNITS_EQ(
      wpi::math::AngleModulus(wpi::units::radian_t{-5 * std::numbers::pi}),
      wpi::units::radian_t{std::numbers::pi});
  EXPECT_UNITS_EQ(
      wpi::math::AngleModulus(wpi::units::radian_t{std::numbers::pi / 2}),
      wpi::units::radian_t{std::numbers::pi / 2});
  EXPECT_UNITS_EQ(
      wpi::math::AngleModulus(wpi::units::radian_t{-std::numbers::pi / 2}),
      wpi::units::radian_t{-std::numbers::pi / 2});
}

TEST(MathUtilTest, IsNear) {
  // The answer is always 42
  // Positive integer checks
  EXPECT_TRUE(wpi::math::IsNear(42, 42, 1));
  EXPECT_TRUE(wpi::math::IsNear(42, 41, 2));
  EXPECT_TRUE(wpi::math::IsNear(42, 43, 2));
  EXPECT_FALSE(wpi::math::IsNear(42, 44, 1));

  // Negative integer checks
  EXPECT_TRUE(wpi::math::IsNear(-42, -42, 1));
  EXPECT_TRUE(wpi::math::IsNear(-42, -41, 2));
  EXPECT_TRUE(wpi::math::IsNear(-42, -43, 2));
  EXPECT_FALSE(wpi::math::IsNear(-42, -44, 1));

  // Mixed sign integer checks
  EXPECT_FALSE(wpi::math::IsNear(-42, 42, 1));
  EXPECT_FALSE(wpi::math::IsNear(-42, 41, 2));
  EXPECT_FALSE(wpi::math::IsNear(-42, 43, 2));
  EXPECT_FALSE(wpi::math::IsNear(42, -42, 1));
  EXPECT_FALSE(wpi::math::IsNear(42, -41, 2));
  EXPECT_FALSE(wpi::math::IsNear(42, -43, 2));

  // Floating point checks
  EXPECT_TRUE(wpi::math::IsNear<double>(42, 41.5, 1));
  EXPECT_TRUE(wpi::math::IsNear<double>(42, 42.5, 1));
  EXPECT_TRUE(wpi::math::IsNear<double>(42, 41.5, 0.75));
  EXPECT_TRUE(wpi::math::IsNear<double>(42, 42.5, 0.75));

  // Wraparound checks
  EXPECT_TRUE(wpi::math::IsNear(0_deg, 356_deg, 5_deg, 0_deg, 360_deg));
  EXPECT_TRUE(wpi::math::IsNear(0, -356, 5, 0, 360));
  EXPECT_TRUE(wpi::math::IsNear(0, 4, 5, 0, 360));
  EXPECT_TRUE(wpi::math::IsNear(0, -4, 5, 0, 360));
  EXPECT_TRUE(wpi::math::IsNear(400, 41, 5, 0, 360));
  EXPECT_TRUE(wpi::math::IsNear(400, -319, 5, 0, 360));
  EXPECT_TRUE(wpi::math::IsNear(400, 401, 5, 0, 360));
  EXPECT_FALSE(wpi::math::IsNear<double>(0, 356, 2.5, 0, 360));
  EXPECT_FALSE(wpi::math::IsNear<double>(0, -356, 2.5, 0, 360));
  EXPECT_FALSE(wpi::math::IsNear<double>(0, 4, 2.5, 0, 360));
  EXPECT_FALSE(wpi::math::IsNear<double>(0, -4, 2.5, 0, 360));
  EXPECT_FALSE(wpi::math::IsNear(400, 35, 5, 0, 360));
  EXPECT_FALSE(wpi::math::IsNear(400, -315, 5, 0, 360));
  EXPECT_FALSE(wpi::math::IsNear(400, 395, 5, 0, 360));
  EXPECT_FALSE(wpi::math::IsNear(0_deg, -4_deg, 2.5_deg, 0_deg, 360_deg));
}

TEST(MathUtilTest, Translation2dSlewRateLimitUnchanged) {
  const wpi::math::Translation2d translation1{0_m, 0_m};
  const wpi::math::Translation2d translation2{2_m, 2_m};

  const wpi::math::Translation2d result1 =
      wpi::math::SlewRateLimit(translation1, translation2, 1_s, 50_mps);

  const wpi::math::Translation2d expected1{2_m, 2_m};

  EXPECT_EQ(result1, expected1);
}

TEST(MathUtilTest, Translation2dSlewRateLimitChanged) {
  const wpi::math::Translation2d translation3{1_m, 1_m};
  const wpi::math::Translation2d translation4{3_m, 3_m};

  const wpi::math::Translation2d result2 =
      wpi::math::SlewRateLimit(translation3, translation4, 0.25_s, 2_mps);

  const wpi::math::Translation2d expected2{
      wpi::units::meter_t{1.0 + 0.5 * (std::numbers::sqrt2 / 2)},
      wpi::units::meter_t{1.0 + 0.5 * (std::numbers::sqrt2 / 2)}};

  EXPECT_EQ(result2, expected2);
}

TEST(MathUtilTest, Translation3dSlewRateLimitUnchanged) {
  const wpi::math::Translation3d translation1{0_m, 0_m, 0_m};
  const wpi::math::Translation3d translation2{2_m, 2_m, 2_m};

  const wpi::math::Translation3d result1 =
      wpi::math::SlewRateLimit(translation1, translation2, 1_s, 50.0_mps);

  const wpi::math::Translation3d expected1{2_m, 2_m, 2_m};

  EXPECT_EQ(result1, expected1);
}

TEST(MathUtilTest, Translation3dSlewRateLimitChanged) {
  const wpi::math::Translation3d translation3{1_m, 1_m, 1_m};
  const wpi::math::Translation3d translation4{3_m, 3_m, 3_m};

  const wpi::math::Translation3d result2 =
      wpi::math::SlewRateLimit(translation3, translation4, 0.25_s, 2.0_mps);

  const wpi::math::Translation3d expected2{
      wpi::units::meter_t{1.0 + 0.5 * std::numbers::inv_sqrt3},
      wpi::units::meter_t{1.0 + 0.5 * std::numbers::inv_sqrt3},
      wpi::units::meter_t{1.0 + 0.5 * std::numbers::inv_sqrt3}};

  EXPECT_EQ(result2, expected2);
}

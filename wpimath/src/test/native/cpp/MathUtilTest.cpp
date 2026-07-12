// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/util/MathUtil.hpp"

#include <limits>
#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

TEST_CASE("MathUtilTest ApplyDeadbandUnityScale", "[wpimath]") {
  // < 0
  CHECK_DOUBLE_EQ(-1.0, wpi::math::ApplyDeadband(-1.0, 0.02));
  CHECK_DOUBLE_EQ((-0.03 + 0.02) / (1.0 - 0.02),
                  wpi::math::ApplyDeadband(-0.03, 0.02));
  CHECK_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(-0.02, 0.02));
  CHECK_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(-0.01, 0.02));

  // == 0
  CHECK_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.0, 0.02));

  // > 0
  CHECK_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.01, 0.02));
  CHECK_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.02, 0.02));
  CHECK_DOUBLE_EQ((0.03 - 0.02) / (1.0 - 0.02),
                  wpi::math::ApplyDeadband(0.03, 0.02));
  CHECK_DOUBLE_EQ(1.0, wpi::math::ApplyDeadband(1.0, 0.02));
}

TEST_CASE("MathUtilTest ApplyDeadbandArbitraryScale", "[wpimath]") {
  // < 0
  CHECK_DOUBLE_EQ(-2.5, wpi::math::ApplyDeadband(-2.5, 0.02, 2.5));
  CHECK_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(-0.02, 0.02, 2.5));
  CHECK_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(-0.01, 0.02, 2.5));

  // == 0
  CHECK_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.0, 0.02, 2.5));

  // > 0
  CHECK_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.01, 0.02, 2.5));
  CHECK_DOUBLE_EQ(0.0, wpi::math::ApplyDeadband(0.02, 0.02, 2.5));
  CHECK_DOUBLE_EQ(2.5, wpi::math::ApplyDeadband(2.5, 0.02, 2.5));
}

TEST_CASE("MathUtilTest ApplyDeadbandUnits", "[wpimath]") {
  // < 0
  CHECK_UNITS_EQ(-20_rad, wpi::math::ApplyDeadband<wpi::units::radian_t>(
                              -20_rad, 1_rad, 20_rad));
}

TEST_CASE("MathUtilTest ApplyDeadbandLargeMaxMagnitude", "[wpimath]") {
  CHECK_DOUBLE_EQ(
      80.0, wpi::math::ApplyDeadband(100.0, 20.0,
                                     std::numeric_limits<double>::infinity()));
}

TEST_CASE("MathUtilTest ApplyDeadband2dUnityScale", "[wpimath]") {
  CHECK((Eigen::Vector2d{{0.0}, {1.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {1.0}}, 0.02));
  CHECK((Eigen::Vector2d{{0.0}, {-1.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {-1.0}}, 0.02));
  CHECK((Eigen::Vector2d{{-1.0}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{-1.0}, {0.0}}, 0.02));

  // == 0
  CHECK((Eigen::Vector2d{{0.0}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {0.0}}, 0.02));

  // > 0
  CHECK((Eigen::Vector2d{{0.0}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.01}, {0.0}}, 0.02));
  CHECK((Eigen::Vector2d{{0.0}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.02}, {0.0}}, 0.02));
  CHECK((Eigen::Vector2d{{(0.03 - 0.02) / (1.0 - 0.02)}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.03}, {0.0}}, 0.02));
  CHECK((Eigen::Vector2d{{1.0}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{1.0}, {0.0}}, 0.02));
}

TEST_CASE("MathUtilTest ApplyDeadband2dArbitraryScale", "[wpimath]") {
  CHECK((Eigen::Vector2d{{0.0}, {2.5}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {2.5}}, 0.02, 2.5));
  CHECK((Eigen::Vector2d{{0.0}, {-2.5}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {-2.5}}, 0.02, 2.5));
  CHECK((Eigen::Vector2d{{-2.5}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{-2.5}, {0.0}}, 0.02, 2.5));

  // == 0
  CHECK((Eigen::Vector2d{{0.0}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.0}, {0.0}}, 0.02, 2.5));

  // > 0
  CHECK((Eigen::Vector2d{{0.0}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.01}, {0.0}}, 0.02, 2.5));
  CHECK((Eigen::Vector2d{{0.0}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{0.02}, {0.0}}, 0.02, 2.5));
  CHECK((Eigen::Vector2d{{2.5}, {0.0}}) ==
        wpi::math::ApplyDeadband(Eigen::Vector2d{{2.5}, {0.0}}, 0.02, 2.5));
}

TEST_CASE("MathUtilTest ApplyDeadband2dLargeMaxMagnitude", "[wpimath]") {
  CHECK((Eigen::Vector2d{{80.0}, {0.0}}) ==
        (wpi::math::ApplyDeadband(Eigen::Vector2d{{100.0}, {0.0}}, 20.0,
                                  std::numeric_limits<double>::infinity())));
}

TEST_CASE("MathUtilTest ApplyDeadband2dUnits", "[wpimath]") {
  CHECK(((Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 2.5_mps}) ==
         wpi::math::ApplyDeadband(
             Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 2.5_mps},
             0.02_mps, 2.5_mps)));
  CHECK(((Eigen::Vector<wpi::units::meters_per_second_t, 2>{1_mps, 0_mps}) ==
         wpi::math::ApplyDeadband(
             Eigen::Vector<wpi::units::meters_per_second_t, 2>{1_mps, 0_mps},
             0.02_mps)));

  CHECK(((Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 0_mps}) ==
         wpi::math::ApplyDeadband(
             Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 0_mps},
             0.02_mps, 2.5_mps)));
}

TEST_CASE("MathUtilTest CopyDirectionPow", "[wpimath]") {
  CHECK_DOUBLE_EQ(0.5, wpi::math::CopyDirectionPow(0.5, 1.0));
  CHECK_DOUBLE_EQ(-0.5, wpi::math::CopyDirectionPow(-0.5, 1.0));

  CHECK_DOUBLE_EQ(0.5 * 0.5, wpi::math::CopyDirectionPow(0.5, 2.0));
  CHECK_DOUBLE_EQ(-(0.5 * 0.5), wpi::math::CopyDirectionPow(-0.5, 2.0));

  CHECK_DOUBLE_EQ(std::sqrt(0.5), wpi::math::CopyDirectionPow(0.5, 0.5));
  CHECK_DOUBLE_EQ(-std::sqrt(0.5), wpi::math::CopyDirectionPow(-0.5, 0.5));

  CHECK_DOUBLE_EQ(0.0, wpi::math::CopyDirectionPow(0.0, 2.0));
  CHECK_DOUBLE_EQ(1.0, wpi::math::CopyDirectionPow(1.0, 2.0));
  CHECK_DOUBLE_EQ(-1.0, wpi::math::CopyDirectionPow(-1.0, 2.0));

  CHECK_DOUBLE_EQ(std::pow(0.8, 0.3), wpi::math::CopyDirectionPow(0.8, 0.3));
  CHECK_DOUBLE_EQ(-std::pow(0.8, 0.3), wpi::math::CopyDirectionPow(-0.8, 0.3));
}

TEST_CASE("MathUtilTest CopyDirectionPowWithMaxMagnitude", "[wpimath]") {
  CHECK_DOUBLE_EQ(5.0, wpi::math::CopyDirectionPow(5.0, 1.0, 10.0));
  CHECK_DOUBLE_EQ(-5.0, wpi::math::CopyDirectionPow(-5.0, 1.0, 10.0));

  CHECK_DOUBLE_EQ(0.5 * 0.5 * 10, wpi::math::CopyDirectionPow(5.0, 2.0, 10.0));
  CHECK_DOUBLE_EQ(-0.5 * 0.5 * 10,
                  wpi::math::CopyDirectionPow(-5.0, 2.0, 10.0));

  CHECK_DOUBLE_EQ(std::sqrt(0.5) * 10,
                  wpi::math::CopyDirectionPow(5.0, 0.5, 10.0));
  CHECK_DOUBLE_EQ(-std::sqrt(0.5) * 10,
                  wpi::math::CopyDirectionPow(-5.0, 0.5, 10.0));

  CHECK_DOUBLE_EQ(0.0, wpi::math::CopyDirectionPow(0.0, 2.0, 5.0));
  CHECK_DOUBLE_EQ(5.0, wpi::math::CopyDirectionPow(5.0, 2.0, 5.0));
  CHECK_DOUBLE_EQ(-5.0, wpi::math::CopyDirectionPow(-5.0, 2.0, 5.0));

  CHECK_DOUBLE_EQ(std::pow(0.8, 0.3) * 100,
                  wpi::math::CopyDirectionPow(80.0, 0.3, 100.0));
  CHECK_DOUBLE_EQ(-std::pow(0.8, 0.3) * 100,
                  wpi::math::CopyDirectionPow(-80.0, 0.3, 100.0));
}

TEST_CASE("MathUtilTest CopyDirectionPowWithUnits", "[wpimath]") {
  CHECK_UNITS_EQ(
      0_mps,
      wpi::math::CopyDirectionPow<wpi::units::meters_per_second_t>(0_mps, 2.0));
  CHECK_UNITS_EQ(
      1_mps,
      wpi::math::CopyDirectionPow<wpi::units::meters_per_second_t>(1_mps, 2.0));
  CHECK_UNITS_EQ(-1_mps,
                 wpi::math::CopyDirectionPow<wpi::units::meters_per_second_t>(
                     -1_mps, 2.0));

  CHECK_UNITS_EQ(wpi::units::meters_per_second_t{0.5 * 0.5 * 10},
                 wpi::math::CopyDirectionPow<wpi::units::meters_per_second_t>(
                     5_mps, 2.0, 10_mps));
  CHECK_UNITS_EQ(wpi::units::meters_per_second_t{-0.5 * 0.5 * 10},
                 wpi::math::CopyDirectionPow<wpi::units::meters_per_second_t>(
                     -5_mps, 2.0, 10_mps));
}

TEST_CASE("MathUtilTest CopyDirectionPow2d", "[wpimath]") {
  CHECK((Eigen::Vector2d{{0.5}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.5}, {0.0}}, 1.0));
  CHECK((Eigen::Vector2d{{-0.5}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{-0.5}, {0.0}}, 1.0));

  CHECK((Eigen::Vector2d{{0.25}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.5}, {0.0}}, 2.0));
  CHECK((Eigen::Vector2d{{-0.25}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{-0.5}, {0.0}}, 2.0));

  CHECK((Eigen::Vector2d{{std::sqrt(0.5)}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.5}, {0.0}}, 0.5));
  CHECK((Eigen::Vector2d{{-std::sqrt(0.5)}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{-0.5}, {0.0}}, 0.5));

  CHECK((Eigen::Vector2d{{0.0}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {0.0}}, 2.0));

  CHECK((Eigen::Vector2d{{1.0}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{1.0}, {0.0}}, 2.0));
  CHECK((Eigen::Vector2d{{-1.0}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{-1.0}, {0.0}}, 2.0));
  CHECK((Eigen::Vector2d{{0.0}, {1.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {1.0}}, 2.0));
  CHECK((Eigen::Vector2d{{0.0}, {-1.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {-1.0}}, 2.0));

  CHECK((Eigen::Vector2d{{0.0}, {std::pow(0.8, 0.3)}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {0.8}}, 0.3));
  CHECK((Eigen::Vector2d{{0.0}, {-std::pow(0.8, 0.3)}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {-0.8}}, 0.3));
}

TEST_CASE("MathUtilTest CopyDirectionPow2dMaxDistance", "[wpimath]") {
  CHECK((Eigen::Vector2d{{5.0}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{5.0}, {0.0}}, 1.0, 10.0));
  CHECK((Eigen::Vector2d{{-5.0}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{-5.0}, {0.0}}, 1.0, 10.0));

  CHECK((Eigen::Vector2d{{2.5}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{5.0}, {0.0}}, 2.0, 10.0));
  CHECK((Eigen::Vector2d{{-2.5}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{-5.0}, {0.0}}, 2.0, 10.0));

  CHECK((Eigen::Vector2d{{std::sqrt(0.5) * 10.0}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{5.0}, {0.0}}, 0.5, 10.0));
  CHECK((Eigen::Vector2d{{-std::sqrt(0.5) * 10.0}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{-5.0}, {0.0}}, 0.5, 10.0));

  CHECK((Eigen::Vector2d{{0.0}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {0.0}}, 2.0, 5.0));
  CHECK((Eigen::Vector2d{{5.0}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{5.0}, {0.0}}, 2.0, 5.0));
  CHECK((Eigen::Vector2d{{-5.0}, {0.0}}) ==
        wpi::math::CopyDirectionPow(Eigen::Vector2d{{-5.0}, {0.0}}, 2.0, 5.0));

  CHECK(
      (Eigen::Vector2d{{0.0}, {std::pow(0.8, 0.3) * 100.0}}) ==
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {80.0}}, 0.3, 100.0));
  CHECK(
      (Eigen::Vector2d{{0.0}, {-std::pow(0.8, 0.3) * 100.0}}) ==
      wpi::math::CopyDirectionPow(Eigen::Vector2d{{0.0}, {-80.0}}, 0.3, 100.0));
}

TEST_CASE("MathUtilTest CopyDirectionPow2dUnits", "[wpimath]") {
  CHECK(((Eigen::Vector<wpi::units::meters_per_second_t, 2>{1_mps, 0_mps}) ==
         wpi::math::CopyDirectionPow(
             Eigen::Vector<wpi::units::meters_per_second_t, 2>{1_mps, 0_mps},
             2.0)));
  CHECK(((Eigen::Vector<wpi::units::meters_per_second_t, 2>{-1_mps, 0_mps}) ==
         wpi::math::CopyDirectionPow(
             Eigen::Vector<wpi::units::meters_per_second_t, 2>{-1_mps, 0_mps},
             2.0)));

  CHECK(((Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 0_mps}) ==
         wpi::math::CopyDirectionPow(
             Eigen::Vector<wpi::units::meters_per_second_t, 2>{0_mps, 0_mps},
             2.0, 5_mps)));

  CHECK(((Eigen::Vector<wpi::units::meters_per_second_t, 2>{5_mps, 0_mps}) ==
         wpi::math::CopyDirectionPow(
             Eigen::Vector<wpi::units::meters_per_second_t, 2>{5_mps, 0_mps},
             2.0, 5_mps)));
  CHECK(((Eigen::Vector<wpi::units::meters_per_second_t, 2>{-5_mps, 0_mps}) ==
         wpi::math::CopyDirectionPow(
             Eigen::Vector<wpi::units::meters_per_second_t, 2>{-5_mps, 0_mps},
             2.0, 5_mps)));
}

TEST_CASE("MathUtilTest InputModulus", "[wpimath]") {
  // These tests check error wrapping. That is, the result of wrapping the
  // result of an angle reference minus the measurement.

  // Test symmetric range
  CHECK_DOUBLE_EQ(-20.0,
                  wpi::math::InputModulus(170.0 - (-170.0), -180.0, 180.0));
  CHECK_DOUBLE_EQ(
      -20.0, wpi::math::InputModulus(170.0 + 360.0 - (-170.0), -180.0, 180.0));
  CHECK_DOUBLE_EQ(
      -20.0, wpi::math::InputModulus(170.0 - (-170.0 + 360.0), -180.0, 180.0));
  CHECK_DOUBLE_EQ(20.0, wpi::math::InputModulus(-170.0 - 170.0, -180.0, 180.0));
  CHECK_DOUBLE_EQ(
      20.0, wpi::math::InputModulus(-170.0 + 360.0 - 170.0, -180.0, 180.0));
  CHECK_DOUBLE_EQ(
      20.0, wpi::math::InputModulus(-170.0 - (170.0 + 360.0), -180.0, 180.0));

  // Test range starting at zero
  CHECK_DOUBLE_EQ(340.0, wpi::math::InputModulus(170.0 - 190.0, 0.0, 360.0));
  CHECK_DOUBLE_EQ(340.0,
                  wpi::math::InputModulus(170.0 + 360.0 - 190.0, 0.0, 360.0));
  CHECK_DOUBLE_EQ(340.0,
                  wpi::math::InputModulus(170.0 - (190.0 + 360.0), 0.0, 360.0));

  // Test asymmetric range that doesn't start at zero
  CHECK_DOUBLE_EQ(-20.0,
                  wpi::math::InputModulus(170.0 - (-170.0), -170.0, 190.0));

  // Test range with both positive endpoints
  CHECK_DOUBLE_EQ(2.0, wpi::math::InputModulus(0.0, 1.0, 3.0));
  CHECK_DOUBLE_EQ(3.0, wpi::math::InputModulus(1.0, 1.0, 3.0));
  CHECK_DOUBLE_EQ(2.0, wpi::math::InputModulus(2.0, 1.0, 3.0));
  CHECK_DOUBLE_EQ(3.0, wpi::math::InputModulus(3.0, 1.0, 3.0));
  CHECK_DOUBLE_EQ(2.0, wpi::math::InputModulus(4.0, 1.0, 3.0));

  // Test all supported types
  CHECK_DOUBLE_EQ(
      -20.0, wpi::math::InputModulus<double>(170.0 - (-170.0), -170.0, 190.0));
  CHECK(-20 == wpi::math::InputModulus<int>(170 - (-170), -170, 190));
  CHECK(-20_deg == wpi::math::InputModulus<wpi::units::degree_t>(
                       170_deg - (-170_deg), -170_deg, 190_deg));
}

TEST_CASE("MathUtilTest AngleModulus", "[wpimath]") {
  CHECK_UNITS_NEAR(wpi::math::AngleModulus(
                       wpi::units::radian_t{-2000 * std::numbers::pi / 180}),
                   wpi::units::radian_t{160 * std::numbers::pi / 180}, 1e-10);
  CHECK_UNITS_NEAR(wpi::math::AngleModulus(
                       wpi::units::radian_t{358 * std::numbers::pi / 180}),
                   wpi::units::radian_t{-2 * std::numbers::pi / 180}, 1e-10);
  CHECK_UNITS_NEAR(
      wpi::math::AngleModulus(wpi::units::radian_t{2.0 * std::numbers::pi}),
      0_rad, 1e-10);

  CHECK_UNITS_EQ(
      wpi::math::AngleModulus(wpi::units::radian_t{5 * std::numbers::pi}),
      wpi::units::radian_t{std::numbers::pi});
  CHECK_UNITS_EQ(
      wpi::math::AngleModulus(wpi::units::radian_t{-5 * std::numbers::pi}),
      wpi::units::radian_t{std::numbers::pi});
  CHECK_UNITS_EQ(
      wpi::math::AngleModulus(wpi::units::radian_t{std::numbers::pi / 2}),
      wpi::units::radian_t{std::numbers::pi / 2});
  CHECK_UNITS_EQ(
      wpi::math::AngleModulus(wpi::units::radian_t{-std::numbers::pi / 2}),
      wpi::units::radian_t{-std::numbers::pi / 2});
}

TEST_CASE("MathUtilTest IsNear", "[wpimath]") {
  // The answer is always 42
  // Positive integer checks
  CHECK(wpi::math::IsNear(42, 42, 1));
  CHECK(wpi::math::IsNear(42, 41, 2));
  CHECK(wpi::math::IsNear(42, 43, 2));
  CHECK_FALSE(wpi::math::IsNear(42, 44, 1));

  // Negative integer checks
  CHECK(wpi::math::IsNear(-42, -42, 1));
  CHECK(wpi::math::IsNear(-42, -41, 2));
  CHECK(wpi::math::IsNear(-42, -43, 2));
  CHECK_FALSE(wpi::math::IsNear(-42, -44, 1));

  // Mixed sign integer checks
  CHECK_FALSE(wpi::math::IsNear(-42, 42, 1));
  CHECK_FALSE(wpi::math::IsNear(-42, 41, 2));
  CHECK_FALSE(wpi::math::IsNear(-42, 43, 2));
  CHECK_FALSE(wpi::math::IsNear(42, -42, 1));
  CHECK_FALSE(wpi::math::IsNear(42, -41, 2));
  CHECK_FALSE(wpi::math::IsNear(42, -43, 2));

  // Floating point checks
  CHECK(wpi::math::IsNear<double>(42, 41.5, 1));
  CHECK(wpi::math::IsNear<double>(42, 42.5, 1));
  CHECK(wpi::math::IsNear<double>(42, 41.5, 0.75));
  CHECK(wpi::math::IsNear<double>(42, 42.5, 0.75));

  // Wraparound checks
  CHECK(wpi::math::IsNear(0_deg, 356_deg, 5_deg, 0_deg, 360_deg));
  CHECK(wpi::math::IsNear(0, -356, 5, 0, 360));
  CHECK(wpi::math::IsNear(0, 4, 5, 0, 360));
  CHECK(wpi::math::IsNear(0, -4, 5, 0, 360));
  CHECK(wpi::math::IsNear(400, 41, 5, 0, 360));
  CHECK(wpi::math::IsNear(400, -319, 5, 0, 360));
  CHECK(wpi::math::IsNear(400, 401, 5, 0, 360));
  CHECK_FALSE(wpi::math::IsNear<double>(0, 356, 2.5, 0, 360));
  CHECK_FALSE(wpi::math::IsNear<double>(0, -356, 2.5, 0, 360));
  CHECK_FALSE(wpi::math::IsNear<double>(0, 4, 2.5, 0, 360));
  CHECK_FALSE(wpi::math::IsNear<double>(0, -4, 2.5, 0, 360));
  CHECK_FALSE(wpi::math::IsNear(400, 35, 5, 0, 360));
  CHECK_FALSE(wpi::math::IsNear(400, -315, 5, 0, 360));
  CHECK_FALSE(wpi::math::IsNear(400, 395, 5, 0, 360));
  CHECK_FALSE(wpi::math::IsNear(0_deg, -4_deg, 2.5_deg, 0_deg, 360_deg));
}

TEST_CASE("MathUtilTest Translation2dSlewRateLimitUnchanged", "[wpimath]") {
  const wpi::math::Translation2d translation1{0_m, 0_m};
  const wpi::math::Translation2d translation2{2_m, 2_m};

  const wpi::math::Translation2d result1 =
      wpi::math::SlewRateLimit(translation1, translation2, 1_s, 50_mps);

  const wpi::math::Translation2d expected1{2_m, 2_m};

  CHECK(result1 == expected1);
}

TEST_CASE("MathUtilTest Translation2dSlewRateLimitChanged", "[wpimath]") {
  const wpi::math::Translation2d translation3{1_m, 1_m};
  const wpi::math::Translation2d translation4{3_m, 3_m};

  const wpi::math::Translation2d result2 =
      wpi::math::SlewRateLimit(translation3, translation4, 0.25_s, 2_mps);

  const wpi::math::Translation2d expected2{
      wpi::units::meter_t{1.0 + 0.5 * (std::numbers::sqrt2 / 2)},
      wpi::units::meter_t{1.0 + 0.5 * (std::numbers::sqrt2 / 2)}};

  CHECK(result2 == expected2);
}

TEST_CASE("MathUtilTest Translation3dSlewRateLimitUnchanged", "[wpimath]") {
  const wpi::math::Translation3d translation1{0_m, 0_m, 0_m};
  const wpi::math::Translation3d translation2{2_m, 2_m, 2_m};

  const wpi::math::Translation3d result1 =
      wpi::math::SlewRateLimit(translation1, translation2, 1_s, 50.0_mps);

  const wpi::math::Translation3d expected1{2_m, 2_m, 2_m};

  CHECK(result1 == expected1);
}

TEST_CASE("MathUtilTest Translation3dSlewRateLimitChanged", "[wpimath]") {
  const wpi::math::Translation3d translation3{1_m, 1_m, 1_m};
  const wpi::math::Translation3d translation4{3_m, 3_m, 3_m};

  const wpi::math::Translation3d result2 =
      wpi::math::SlewRateLimit(translation3, translation4, 0.25_s, 2.0_mps);

  const wpi::math::Translation3d expected2{
      wpi::units::meter_t{1.0 + 0.5 * std::numbers::inv_sqrt3},
      wpi::units::meter_t{1.0 + 0.5 * std::numbers::inv_sqrt3},
      wpi::units::meter_t{1.0 + 0.5 * std::numbers::inv_sqrt3}};

  CHECK(result2 == expected2);
}

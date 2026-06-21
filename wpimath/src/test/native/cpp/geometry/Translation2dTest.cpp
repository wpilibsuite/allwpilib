// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Translation2d.hpp"

#include <cmath>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"

using namespace wpi::math;

TEST_CASE("Translation2dTest Sum", "[wpimath]") {
  const Translation2d one{1_m, 3_m};
  const Translation2d two{2_m, 5_m};

  const auto sum = one + two;

  CHECK_DOUBLE_EQ(3.0, sum.X().value());
  CHECK_DOUBLE_EQ(8.0, sum.Y().value());
}

TEST_CASE("Translation2dTest Difference", "[wpimath]") {
  const Translation2d one{1_m, 3_m};
  const Translation2d two{2_m, 5_m};

  const auto difference = one - two;

  CHECK_DOUBLE_EQ(-1.0, difference.X().value());
  CHECK_DOUBLE_EQ(-2.0, difference.Y().value());
}

TEST_CASE("Translation2dTest RotateBy", "[wpimath]") {
  const Translation2d another{3_m, 0_m};
  const auto rotated = another.RotateBy(90_deg);

  CHECK_NEAR(0.0, rotated.X().value(), 1e-9);
  CHECK_NEAR(3.0, rotated.Y().value(), 1e-9);
}

TEST_CASE("Translation2dTest RotateAround", "[wpimath]") {
  const Translation2d translation{2_m, 1_m};
  const Translation2d other{3_m, 2_m};
  const auto rotated = translation.RotateAround(other, 180_deg);

  CHECK_NEAR(4.0, rotated.X().value(), 1e-9);
  CHECK_NEAR(3.0, rotated.Y().value(), 1e-9);
}

TEST_CASE("Translation2dTest Multiplication", "[wpimath]") {
  const Translation2d original{3_m, 5_m};
  const auto mult = original * 3;

  CHECK_DOUBLE_EQ(9.0, mult.X().value());
  CHECK_DOUBLE_EQ(15.0, mult.Y().value());
}

TEST_CASE("Translation2dTest Division", "[wpimath]") {
  const Translation2d original{3_m, 5_m};
  const auto div = original / 2;

  CHECK_DOUBLE_EQ(1.5, div.X().value());
  CHECK_DOUBLE_EQ(2.5, div.Y().value());
}

TEST_CASE("Translation2dTest Norm", "[wpimath]") {
  const Translation2d one{3_m, 5_m};
  CHECK_DOUBLE_EQ(std::hypot(3.0, 5.0), one.Norm().value());
}

TEST_CASE("Translation2dTest SquaredNorm", "[wpimath]") {
  const Translation2d one{3_m, 5_m};
  CHECK_DOUBLE_EQ(34.0, one.SquaredNorm().value());
}

TEST_CASE("Translation2dTest Distance", "[wpimath]") {
  const Translation2d one{1_m, 1_m};
  const Translation2d two{6_m, 6_m};
  CHECK_DOUBLE_EQ(5.0 * std::sqrt(2.0), one.Distance(two).value());
}

TEST_CASE("Translation2dTest SquaredDistance", "[wpimath]") {
  const Translation2d one{1_m, 1_m};
  const Translation2d two{6_m, 6_m};
  CHECK_DOUBLE_EQ(50.0, one.SquaredDistance(two).value());
}

TEST_CASE("Translation2dTest UnaryMinus", "[wpimath]") {
  const Translation2d original{-4.5_m, 7_m};
  const auto inverted = -original;

  CHECK_DOUBLE_EQ(4.5, inverted.X().value());
  CHECK_DOUBLE_EQ(-7.0, inverted.Y().value());
}

TEST_CASE("Translation2dTest Equality", "[wpimath]") {
  const Translation2d one{9_m, 5.5_m};
  const Translation2d two{9_m, 5.5_m};
  CHECK(one == two);
}

TEST_CASE("Translation2dTest Inequality", "[wpimath]") {
  const Translation2d one{9_m, 5.5_m};
  const Translation2d two{9_m, 5.7_m};
  CHECK(one != two);
}

TEST_CASE("Translation2dTest PolarConstructor", "[wpimath]") {
  Translation2d one{std::sqrt(2) * 1_m, Rotation2d{45_deg}};
  CHECK_DOUBLE_EQ(1.0, one.X().value());
  CHECK_DOUBLE_EQ(1.0, one.Y().value());

  Translation2d two{2_m, Rotation2d{60_deg}};
  CHECK_DOUBLE_EQ(1.0, two.X().value());
  CHECK_DOUBLE_EQ(std::sqrt(3.0), two.Y().value());
}

TEST_CASE("Translation2dTest Nearest", "[wpimath]") {
  const Translation2d origin{0_m, 0_m};

  const Translation2d translation1{1_m, Rotation2d{45_deg}};
  const Translation2d translation2{2_m, Rotation2d{90_deg}};
  const Translation2d translation3{3_m, Rotation2d{135_deg}};
  const Translation2d translation4{4_m, Rotation2d{180_deg}};
  const Translation2d translation5{5_m, Rotation2d{270_deg}};

  CHECK_DOUBLE_EQ(
      origin.Nearest({translation5, translation3, translation4}).X().value(),
      translation3.X().value());
  CHECK_DOUBLE_EQ(
      origin.Nearest({translation5, translation3, translation4}).Y().value(),
      translation3.Y().value());

  CHECK_DOUBLE_EQ(
      origin.Nearest({translation1, translation2, translation3}).X().value(),
      translation1.X().value());
  CHECK_DOUBLE_EQ(
      origin.Nearest({translation1, translation2, translation3}).Y().value(),
      translation1.Y().value());

  CHECK_DOUBLE_EQ(
      origin.Nearest({translation4, translation2, translation3}).X().value(),
      translation2.X().value());
  CHECK_DOUBLE_EQ(
      origin.Nearest({translation4, translation2, translation3}).Y().value(),
      translation2.Y().value());
}

TEST_CASE("Translation2dTest ToVector", "[wpimath]") {
  const Eigen::Vector2d vec(1.0, 2.0);
  const Translation2d translation{vec};

  CHECK_DOUBLE_EQ(vec[0], translation.X().value());
  CHECK_DOUBLE_EQ(vec[1], translation.Y().value());

  CHECK(vec == translation.ToVector());
}

TEST_CASE("Translation2dTest Constexpr", "[wpimath]") {
  constexpr Translation2d defaultCtor;
  constexpr Translation2d componentCtor{1_m, 2_m};
  constexpr auto added = defaultCtor + componentCtor;
  constexpr auto subtracted = defaultCtor - componentCtor;
  constexpr auto negated = -componentCtor;
  constexpr auto multiplied = componentCtor * 2;
  constexpr auto divided = componentCtor / 2;

  static_assert(defaultCtor.X() == 0_m);
  static_assert(componentCtor.Y() == 2_m);
  static_assert(added.X() == 1_m);
  static_assert(subtracted.Y() == (-2_m));
  static_assert(negated.X() == (-1_m));
  static_assert(multiplied.X() == 2_m);
  static_assert(divided.Y() == 1_m);
}

TEST_CASE("Translation2dTest Dot", "[wpimath]") {
  const Translation2d one{2_m, 3_m};
  const Translation2d two{3_m, 4_m};
  CHECK_DOUBLE_EQ(18.0, one.Dot(two).value());
}

TEST_CASE("Translation2dTest Cross", "[wpimath]") {
  const Translation2d one{2_m, 3_m};
  const Translation2d two{3_m, 4_m};
  CHECK_DOUBLE_EQ(-1.0, one.Cross(two).value());
}

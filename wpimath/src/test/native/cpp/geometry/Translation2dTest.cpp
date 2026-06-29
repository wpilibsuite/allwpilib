// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Translation2d.hpp"

#include <cmath>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace wpi::math;

TEST_CASE("Translation2dTest Sum", "[wpimath]") {
  const Translation2d one{1_m, 3_m};
  const Translation2d two{2_m, 5_m};

  const auto sum = one + two;

  CHECK(3.0 == Catch::Approx(sum.X().value()));
  CHECK(8.0 == Catch::Approx(sum.Y().value()));
}

TEST_CASE("Translation2dTest Difference", "[wpimath]") {
  const Translation2d one{1_m, 3_m};
  const Translation2d two{2_m, 5_m};

  const auto difference = one - two;

  CHECK(-1.0 == Catch::Approx(difference.X().value()));
  CHECK(-2.0 == Catch::Approx(difference.Y().value()));
}

TEST_CASE("Translation2dTest RotateBy", "[wpimath]") {
  const Translation2d another{3_m, 0_m};
  const auto rotated = another.RotateBy(90_deg);

  CHECK(0.0 == Catch::Approx(rotated.X().value()).margin(1e-9));
  CHECK(3.0 == Catch::Approx(rotated.Y().value()).margin(1e-9));
}

TEST_CASE("Translation2dTest RotateAround", "[wpimath]") {
  const Translation2d translation{2_m, 1_m};
  const Translation2d other{3_m, 2_m};
  const auto rotated = translation.RotateAround(other, 180_deg);

  CHECK(4.0 == Catch::Approx(rotated.X().value()).margin(1e-9));
  CHECK(3.0 == Catch::Approx(rotated.Y().value()).margin(1e-9));
}

TEST_CASE("Translation2dTest Multiplication", "[wpimath]") {
  const Translation2d original{3_m, 5_m};
  const auto mult = original * 3;

  CHECK(9.0 == Catch::Approx(mult.X().value()));
  CHECK(15.0 == Catch::Approx(mult.Y().value()));
}

TEST_CASE("Translation2dTest Division", "[wpimath]") {
  const Translation2d original{3_m, 5_m};
  const auto div = original / 2;

  CHECK(1.5 == Catch::Approx(div.X().value()));
  CHECK(2.5 == Catch::Approx(div.Y().value()));
}

TEST_CASE("Translation2dTest Norm", "[wpimath]") {
  const Translation2d one{3_m, 5_m};
  CHECK(std::hypot(3.0, 5.0) == Catch::Approx(one.Norm().value()));
}

TEST_CASE("Translation2dTest SquaredNorm", "[wpimath]") {
  const Translation2d one{3_m, 5_m};
  CHECK(34.0 == Catch::Approx(one.SquaredNorm().value()));
}

TEST_CASE("Translation2dTest Distance", "[wpimath]") {
  const Translation2d one{1_m, 1_m};
  const Translation2d two{6_m, 6_m};
  CHECK(5.0 * std::sqrt(2.0) == Catch::Approx(one.Distance(two).value()));
}

TEST_CASE("Translation2dTest SquaredDistance", "[wpimath]") {
  const Translation2d one{1_m, 1_m};
  const Translation2d two{6_m, 6_m};
  CHECK(50.0 == Catch::Approx(one.SquaredDistance(two).value()));
}

TEST_CASE("Translation2dTest UnaryMinus", "[wpimath]") {
  const Translation2d original{-4.5_m, 7_m};
  const auto inverted = -original;

  CHECK(4.5 == Catch::Approx(inverted.X().value()));
  CHECK(-7.0 == Catch::Approx(inverted.Y().value()));
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
  CHECK(1.0 == Catch::Approx(one.X().value()));
  CHECK(1.0 == Catch::Approx(one.Y().value()));

  Translation2d two{2_m, Rotation2d{60_deg}};
  CHECK(1.0 == Catch::Approx(two.X().value()));
  CHECK(std::sqrt(3.0) == Catch::Approx(two.Y().value()));
}

TEST_CASE("Translation2dTest Nearest", "[wpimath]") {
  const Translation2d origin{0_m, 0_m};

  const Translation2d translation1{1_m, Rotation2d{45_deg}};
  const Translation2d translation2{2_m, Rotation2d{90_deg}};
  const Translation2d translation3{3_m, Rotation2d{135_deg}};
  const Translation2d translation4{4_m, Rotation2d{180_deg}};
  const Translation2d translation5{5_m, Rotation2d{270_deg}};

  CHECK(
      origin.Nearest({translation5, translation3, translation4}).X().value() ==
      Catch::Approx(translation3.X().value()));
  CHECK(
      origin.Nearest({translation5, translation3, translation4}).Y().value() ==
      Catch::Approx(translation3.Y().value()));

  CHECK(
      origin.Nearest({translation1, translation2, translation3}).X().value() ==
      Catch::Approx(translation1.X().value()));
  CHECK(
      origin.Nearest({translation1, translation2, translation3}).Y().value() ==
      Catch::Approx(translation1.Y().value()));

  CHECK(
      origin.Nearest({translation4, translation2, translation3}).X().value() ==
      Catch::Approx(translation2.X().value()));
  CHECK(
      origin.Nearest({translation4, translation2, translation3}).Y().value() ==
      Catch::Approx(translation2.Y().value()));
}

TEST_CASE("Translation2dTest ToVector", "[wpimath]") {
  const Eigen::Vector2d vec(1.0, 2.0);
  const Translation2d translation{vec};

  CHECK(vec[0] == Catch::Approx(translation.X().value()));
  CHECK(vec[1] == Catch::Approx(translation.Y().value()));

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
  CHECK(18.0 == Catch::Approx(one.Dot(two).value()));
}

TEST_CASE("Translation2dTest Cross", "[wpimath]") {
  const Translation2d one{2_m, 3_m};
  const Translation2d two{3_m, 4_m};
  CHECK(-1.0 == Catch::Approx(one.Cross(two).value()));
}

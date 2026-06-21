// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose2d.hpp"

using namespace wpi::math;

TEST_CASE("Twist2dTest Straight", "[wpimath]") {
  const Twist2d straight{5_m, 0_m, 0_rad};
  const auto straightTransform = straight.Exp();

  CHECK(5.0 == Catch::Approx(straightTransform.X().value()));
  CHECK(0.0 == Catch::Approx(straightTransform.Y().value()));
  CHECK(0.0 == Catch::Approx(straightTransform.Rotation().Radians().value()));
}

TEST_CASE("Twist2dTest QuarterCircle", "[wpimath]") {
  const Twist2d quarterCircle{5_m / 2.0 * std::numbers::pi, 0_m,
                              wpi::units::radian_t{std::numbers::pi / 2.0}};
  const auto quarterCircleTransform = quarterCircle.Exp();

  CHECK(5.0 == Catch::Approx(quarterCircleTransform.X().value()));
  CHECK(5.0 == Catch::Approx(quarterCircleTransform.Y().value()));
  CHECK(90.0 ==
        Catch::Approx(quarterCircleTransform.Rotation().Degrees().value()));
}

TEST_CASE("Twist2dTest DiagonalNoDtheta", "[wpimath]") {
  const Twist2d diagonal{2_m, 2_m, 0_deg};
  const auto diagonalTransform = diagonal.Exp();

  CHECK(2.0 == Catch::Approx(diagonalTransform.X().value()));
  CHECK(2.0 == Catch::Approx(diagonalTransform.Y().value()));
  CHECK(0.0 == Catch::Approx(diagonalTransform.Rotation().Degrees().value()));
}

TEST_CASE("Twist2dTest Equality", "[wpimath]") {
  const Twist2d one{5_m, 1_m, 3_rad};
  const Twist2d two{5_m, 1_m, 3_rad};
  CHECK(one == two);
}

TEST_CASE("Twist2dTest Inequality", "[wpimath]") {
  const Twist2d one{5_m, 1_m, 3_rad};
  const Twist2d two{5_m, 1.2_m, 3_rad};
  CHECK(one != two);
}

TEST_CASE("Twist2dTest Pose2dLog", "[wpimath]") {
  const Pose2d end{5_m, 5_m, 90_deg};
  const Pose2d start;

  const auto twist = (end - start).Log();

  Twist2d expected{wpi::units::meter_t{5.0 / 2.0 * std::numbers::pi}, 0_m,
                   wpi::units::radian_t{std::numbers::pi / 2.0}};
  CHECK(expected == twist);

  // Make sure computed twist gives back original end pose
  const auto reapplied = start + twist.Exp();
  CHECK(end == reapplied);
}

TEST_CASE("Twist2dTest Constexpr", "[wpimath]") {
  constexpr Twist2d defaultCtor;
  constexpr Twist2d componentCtor{1_m, 2_m, 3_rad};
  constexpr auto multiplied = componentCtor * 2;

  static_assert(defaultCtor.dx == 0_m);
  static_assert(componentCtor.dy == 2_m);
  static_assert(multiplied.dtheta == 6_rad);
}

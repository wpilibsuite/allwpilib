// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Rotation2d.hpp"

#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace wpi::math;

TEST_CASE("Rotation2dTest RadiansToDegrees", "[wpimath]") {
  const Rotation2d rot1{wpi::units::radian_t{std::numbers::pi / 3.0}};
  const Rotation2d rot2{wpi::units::radian_t{std::numbers::pi / 4.0}};

  CHECK(60.0 == Catch::Approx(rot1.Degrees().value()));
  CHECK(45.0 == Catch::Approx(rot2.Degrees().value()));
}

TEST_CASE("Rotation2dTest DegreesToRadians", "[wpimath]") {
  const auto rot1 = Rotation2d{45_deg};
  const auto rot2 = Rotation2d{30_deg};

  CHECK(std::numbers::pi / 4.0 == Catch::Approx(rot1.Radians().value()));
  CHECK(std::numbers::pi / 6.0 == Catch::Approx(rot2.Radians().value()));
}

TEST_CASE("Rotation2dTest RotateByFromZero", "[wpimath]") {
  const Rotation2d zero;
  auto rotated = zero + Rotation2d{90_deg};

  CHECK(std::numbers::pi / 2.0 == Catch::Approx(rotated.Radians().value()));
  CHECK(90.0 == Catch::Approx(rotated.Degrees().value()));
}

TEST_CASE("Rotation2dTest RotateByNonZero", "[wpimath]") {
  auto rot = Rotation2d{90_deg};
  rot = rot + Rotation2d{30_deg};

  CHECK(120.0 == Catch::Approx(rot.Degrees().value()));
}

TEST_CASE("Rotation2dTest RelativeTo", "[wpimath]") {
  auto start = Rotation2d{30_deg};
  auto end = Rotation2d{90_deg};

  auto result = end.RelativeTo(start);

  CHECK(60.0 == Catch::Approx(result.Degrees().value()));
}

TEST_CASE("Rotation2dTest Minus", "[wpimath]") {
  const auto rot1 = Rotation2d{70_deg};
  const auto rot2 = Rotation2d{30_deg};

  CHECK(40.0 == Catch::Approx((rot1 - rot2).Degrees().value()));
}

TEST_CASE("Rotation2dTest UnaryMinus", "[wpimath]") {
  const auto rot = Rotation2d{20_deg};

  CHECK(-20.0 == Catch::Approx((-rot).Degrees().value()));
}

TEST_CASE("Rotation2dTest Multiply", "[wpimath]") {
  const auto rot = Rotation2d{10_deg};

  CHECK(30.0 == Catch::Approx((rot * 3.0).Degrees().value()));
  CHECK(50.0 == Catch::Approx((rot * 41.0).Degrees().value()));
}

TEST_CASE("Rotation2dTest Equality", "[wpimath]") {
  auto rot1 = Rotation2d{43_deg};
  auto rot2 = Rotation2d{43_deg};
  CHECK(rot1 == rot2);

  rot1 = Rotation2d{-180_deg};
  rot2 = Rotation2d{180_deg};
  CHECK(rot1 == rot2);
}

TEST_CASE("Rotation2dTest Inequality", "[wpimath]") {
  const auto rot1 = Rotation2d{43_deg};
  const auto rot2 = Rotation2d{43.5_deg};
  CHECK(rot1 != rot2);
}

TEST_CASE("Rotation2dTest ToMatrix", "[wpimath]") {
#if __GNUC__ <= 11
  Rotation2d before{20_deg};
  Rotation2d after{before.ToMatrix()};
#else
  constexpr Rotation2d before{20_deg};
  constexpr Rotation2d after{before.ToMatrix()};
#endif

  CHECK(before == after);
}

TEST_CASE("Rotation2dTest Constexpr", "[wpimath]") {
  constexpr Rotation2d defaultCtor;
  constexpr Rotation2d radianCtor{5_rad};
  constexpr Rotation2d degreeCtor{270_deg};
  constexpr Rotation2d rotation45{45_deg};
  constexpr Rotation2d cartesianCtor{3.5, -3.5};

  constexpr auto negated = -radianCtor;
  constexpr auto multiplied = radianCtor * 2;
  constexpr auto subtracted = cartesianCtor - degreeCtor;

  static_assert(defaultCtor.Radians() == 0_rad);
  static_assert(degreeCtor.Degrees() == -90_deg);
  static_assert(negated.Radians() == -5_rad + 1_tr);
  static_assert(multiplied.Radians() == 10_rad - 2_tr);
  static_assert(subtracted == rotation45);
  static_assert(radianCtor != degreeCtor);
}

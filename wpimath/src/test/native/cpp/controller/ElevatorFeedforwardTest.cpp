// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/ElevatorFeedforward.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/LinearPlantInversionFeedforward.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

static constexpr auto Ks = 0.5_V;
static constexpr auto Kv = 1.5_V * 1_s / 1_m;
static constexpr auto Ka = 2_V * 1_s * 1_s / 1_m;
static constexpr auto Kg = 1_V;

TEST_CASE("ElevatorFeedforwardTest Calculate", "[wpimath]") {
  wpi::math::ElevatorFeedforward elevatorFF{Ks, Kg, Kv, Ka};

  CHECK(elevatorFF.Calculate(0_m / 1_s).value() ==
        Catch::Approx(Kg.value()).margin(0.002));
  CHECK(elevatorFF.Calculate(2_m / 1_s).value() ==
        Catch::Approx(4.5).margin(0.002));

  wpi::math::Matrixd<1, 1> A{-Kv.value() / Ka.value()};
  wpi::math::Matrixd<1, 1> B{1.0 / Ka.value()};
  constexpr wpi::units::second_t dt = 20_ms;
  wpi::math::LinearPlantInversionFeedforward<1, 1> plantInversion{A, B, dt};

  wpi::math::Vectord<1> r{2.0};
  wpi::math::Vectord<1> nextR{3.0};
  CHECK(
      plantInversion.Calculate(r, nextR)(0) + Ks.value() + Kg.value() ==
      Catch::Approx(elevatorFF.Calculate(2_mps, 3_mps).value()).margin(0.002));
}

TEST_CASE("ElevatorFeedforwardTest AchievableVelocity", "[wpimath]") {
  wpi::math::ElevatorFeedforward elevatorFF{Ks, Kg, Kv, Ka};
  CHECK(elevatorFF.MaxAchievableVelocity(11_V, 1_m / 1_s / 1_s).value() ==
        Catch::Approx(5).margin(0.002));
  CHECK(elevatorFF.MinAchievableVelocity(11_V, 1_m / 1_s / 1_s).value() ==
        Catch::Approx(-9).margin(0.002));
}

TEST_CASE("ElevatorFeedforwardTest AchievableAcceleration", "[wpimath]") {
  wpi::math::ElevatorFeedforward elevatorFF{Ks, Kg, Kv, Ka};
  CHECK(elevatorFF.MaxAchievableAcceleration(12_V, 2_m / 1_s).value() ==
        Catch::Approx(3.75).margin(0.002));
  CHECK(elevatorFF.MaxAchievableAcceleration(12_V, -2_m / 1_s).value() ==
        Catch::Approx(7.25).margin(0.002));
  CHECK(elevatorFF.MinAchievableAcceleration(12_V, 2_m / 1_s).value() ==
        Catch::Approx(-8.25).margin(0.002));
  CHECK(elevatorFF.MinAchievableAcceleration(12_V, -2_m / 1_s).value() ==
        Catch::Approx(-4.75).margin(0.002));
}

TEST_CASE("ElevatorFeedforwardTest NegativeGains", "[wpimath]") {
  wpi::math::ElevatorFeedforward elevatorFF{Ks, Kg, -Kv, -Ka};
  CHECK(elevatorFF.GetKv().value() == 0);
  CHECK(elevatorFF.GetKa().value() == 0);
}

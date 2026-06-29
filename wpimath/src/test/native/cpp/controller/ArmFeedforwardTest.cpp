// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/ArmFeedforward.hpp"

#include <cmath>
#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/voltage.hpp"

namespace {

using Ks_unit = decltype(1_V);
using Kv_unit = decltype(1_V / 1_rad_per_s);
using Ka_unit = decltype(1_V / 1_rad_per_s_sq);
using Kg_unit = decltype(1_V);

/**
 * Simulates a single-jointed arm and returns the final state.
 *
 * @param Ks The static gain, in volts.
 * @param Kv The velocity gain, in volt seconds per radian.
 * @param Ka The acceleration gain, in volt seconds² per radian.
 * @param Kg The gravity gain, in volts.
 * @param currentAngle The starting angle.
 * @param currentVelocity The starting angular velocity.
 * @param input The input voltage.
 * @param dt The simulation time.
 * @return The final state as a 2-vector of angle and angular velocity.
 */
wpi::math::Matrixd<2, 1> Simulate(
    Ks_unit Ks, Kv_unit Kv, Ka_unit Ka, Kg_unit Kg,
    wpi::units::radian_t currentAngle,
    wpi::units::radians_per_second_t currentVelocity, wpi::units::volt_t input,
    wpi::units::second_t dt) {
  wpi::math::Matrixd<2, 2> A{{0.0, 1.0}, {0.0, -Kv.value() / Ka.value()}};
  wpi::math::Matrixd<2, 1> B{{0.0}, {1.0 / Ka.value()}};

  return wpi::math::RK4(
      [&](const wpi::math::Matrixd<2, 1>& x,
          const wpi::math::Matrixd<1, 1>& u) -> wpi::math::Matrixd<2, 1> {
        wpi::math::Matrixd<2, 1> c{
            0.0, -Ks.value() / Ka.value() * wpi::util::sgn(x(1)) -
                     Kg.value() / Ka.value() * std::cos(x(0))};
        return A * x + B * u + c;
      },
      wpi::math::Matrixd<2, 1>{currentAngle.value(), currentVelocity.value()},
      wpi::math::Matrixd<1, 1>{input.value()}, dt);
}

/**
 * Simulates a single-jointed arm and returns the final state.
 *
 * @param armFF The feedforward object.
 * @param Ks The static gain, in volts.
 * @param Kv The velocity gain, in volt seconds per radian.
 * @param Ka The acceleration gain, in volt seconds² per radian.
 * @param Kg The gravity gain, in volts.
 * @param currentAngle The starting angle.
 * @param currentVelocity The starting angular velocity.
 * @param input The input voltage.
 * @param dt The simulation time.
 */
void CalculateAndSimulate(const wpi::math::ArmFeedforward& armFF, Ks_unit Ks,
                          Kv_unit Kv, Ka_unit Ka, Kg_unit Kg,
                          wpi::units::radian_t currentAngle,
                          wpi::units::radians_per_second_t currentVelocity,
                          wpi::units::radians_per_second_t nextVelocity,
                          wpi::units::second_t dt) {
  auto input = armFF.Calculate(currentAngle, currentVelocity, nextVelocity);
  CHECK(nextVelocity.value() ==
        Catch::Approx(Simulate(Ks, Kv, Ka, Kg, currentAngle, currentVelocity,
                               input, dt)(1))
            .margin(1e-4));
}

}  // namespace

TEST_CASE("ArmFeedforwardTest Calculate", "[wpimath]") {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  // Calculate(angle, angular velocity)
  CHECK(armFF.Calculate(std::numbers::pi / 3 * 1_rad, 0_rad_per_s).value() ==
        Catch::Approx(0.5).margin(0.002));
  CHECK(armFF.Calculate(std::numbers::pi / 3 * 1_rad, 1_rad_per_s).value() ==
        Catch::Approx(2.5).margin(0.002));

  // Calculate(currentAngle, currentVelocity, nextAngle, dt)
  CalculateAndSimulate(armFF, Ks, Kv, Ka, Kg, std::numbers::pi / 3 * 1_rad,
                       1_rad_per_s, 1.05_rad_per_s, 20_ms);
  CalculateAndSimulate(armFF, Ks, Kv, Ka, Kg, std::numbers::pi / 3 * 1_rad,
                       1_rad_per_s, 0.95_rad_per_s, 20_ms);
  CalculateAndSimulate(armFF, Ks, Kv, Ka, Kg, -std::numbers::pi / 3 * 1_rad,
                       1_rad_per_s, 1.05_rad_per_s, 20_ms);
  CalculateAndSimulate(armFF, Ks, Kv, Ka, Kg, -std::numbers::pi / 3 * 1_rad,
                       1_rad_per_s, 0.95_rad_per_s, 20_ms);
}

TEST_CASE("ArmFeedforwardTest CalculateIllConditionedModel", "[wpimath]") {
  constexpr auto Ks = 0.39671_V;
  constexpr auto Kv = 2.7167_V / 1_rad_per_s;
  constexpr auto Ka = 1e-2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 0.2708_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  constexpr auto currentAngle = 1_rad;
  constexpr auto currentVelocity = 0.02_rad_per_s;
  constexpr auto nextVelocity = 0_rad_per_s;

  constexpr auto averageAccel = (nextVelocity - currentVelocity) / 20_ms;

  CHECK(armFF.Calculate(currentAngle, currentVelocity, nextVelocity).value() ==
        Catch::Approx((Ks + Kv * currentVelocity + Ka * averageAccel +
                       Kg * wpi::units::math::cos(currentAngle))
                          .value()));
}

TEST_CASE("ArmFeedforwardTest CalculateIllConditionedGradient", "[wpimath]") {
  constexpr auto Ks = 0.39671_V;
  constexpr auto Kv = 2.7167_V / 1_rad_per_s;
  constexpr auto Ka = 0.50799_V / 1_rad_per_s_sq;
  constexpr auto Kg = 0.2708_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  CalculateAndSimulate(armFF, Ks, Kv, Ka, Kg, 1_rad, 0.02_rad_per_s,
                       0_rad_per_s, 20_ms);
}

TEST_CASE("ArmFeedforwardTest AchievableVelocity", "[wpimath]") {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  CHECK(armFF
            .MaxAchievableVelocity(12_V, std::numbers::pi / 3 * 1_rad,
                                   1_rad_per_s_sq)
            .value() == Catch::Approx(6).margin(0.002));
  CHECK(armFF
            .MinAchievableVelocity(11.5_V, std::numbers::pi / 3 * 1_rad,
                                   1_rad_per_s_sq)
            .value() == Catch::Approx(-9).margin(0.002));
}

TEST_CASE("ArmFeedforwardTest AchievableAcceleration", "[wpimath]") {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  CHECK(armFF
            .MaxAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                       1_rad_per_s)
            .value() == Catch::Approx(4.75).margin(0.002));
  CHECK(armFF
            .MaxAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                       -1_rad_per_s)
            .value() == Catch::Approx(6.75).margin(0.002));
  CHECK(armFF
            .MinAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                       1_rad_per_s)
            .value() == Catch::Approx(-7.25).margin(0.002));
  CHECK(armFF
            .MinAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                       -1_rad_per_s)
            .value() == Catch::Approx(-5.25).margin(0.002));
}

TEST_CASE("ArmFeedforwardTest NegativeGains", "[wpimath]") {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, -Kv, -Ka};

  CHECK(armFF.GetKv().value() == 0);
  CHECK(armFF.GetKa().value() == 0);
}

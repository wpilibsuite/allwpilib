// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>

#include <gtest/gtest.h>
#include <wpi/math/EigenCore.h>
#include <wpi/math/controller/ArmFeedforward.h>
#include <wpi/math/system/NumericalIntegration.h>

#include "units/angular_acceleration.h"
#include "units/angular_velocity.h"
#include "units/time.h"
#include "units/voltage.h"

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
wpi::math::Matrixd<2, 1> Simulate(Ks_unit Ks, Kv_unit Kv, Ka_unit Ka,
                                  Kg_unit Kg, units::radian_t currentAngle,
                                  units::radians_per_second_t currentVelocity,
                                  units::volt_t input, units::second_t dt) {
  wpi::math::Matrixd<2, 2> A{{0.0, 1.0}, {0.0, -Kv.value() / Ka.value()}};
  wpi::math::Matrixd<2, 1> B{{0.0}, {1.0 / Ka.value()}};

  return wpi::math::RK4(
      [&](const wpi::math::Matrixd<2, 1>& x,
          const wpi::math::Matrixd<1, 1>& u) -> wpi::math::Matrixd<2, 1> {
        wpi::math::Matrixd<2, 1> c{
            0.0, -Ks.value() / Ka.value() * wpi::sgn(x(1)) -
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
                          units::radian_t currentAngle,
                          units::radians_per_second_t currentVelocity,
                          units::radians_per_second_t nextVelocity,
                          units::second_t dt) {
  auto input = armFF.Calculate(currentAngle, currentVelocity, nextVelocity);
  EXPECT_NEAR(
      nextVelocity.value(),
      Simulate(Ks, Kv, Ka, Kg, currentAngle, currentVelocity, input, dt)(1),
      1e-4);
}

}  // namespace

TEST(ArmFeedforwardTest, Calculate) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  // Calculate(angle, angular velocity)
  EXPECT_NEAR(
      armFF.Calculate(std::numbers::pi / 3 * 1_rad, 0_rad_per_s).value(), 0.5,
      0.002);
  EXPECT_NEAR(
      armFF.Calculate(std::numbers::pi / 3 * 1_rad, 1_rad_per_s).value(), 2.5,
      0.002);

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

TEST(ArmFeedforwardTest, CalculateIllConditionedModel) {
  constexpr auto Ks = 0.39671_V;
  constexpr auto Kv = 2.7167_V / 1_rad_per_s;
  constexpr auto Ka = 1e-2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 0.2708_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  constexpr auto currentAngle = 1_rad;
  constexpr auto currentVelocity = 0.02_rad_per_s;
  constexpr auto nextVelocity = 0_rad_per_s;

  constexpr auto averageAccel = (nextVelocity - currentVelocity) / 20_ms;

  EXPECT_DOUBLE_EQ(
      armFF.Calculate(currentAngle, currentVelocity, nextVelocity).value(),
      (Ks + Kv * currentVelocity + Ka * averageAccel +
       Kg * units::math::cos(currentAngle))
          .value());
}

TEST(ArmFeedforwardTest, CalculateIllConditionedGradient) {
  constexpr auto Ks = 0.39671_V;
  constexpr auto Kv = 2.7167_V / 1_rad_per_s;
  constexpr auto Ka = 0.50799_V / 1_rad_per_s_sq;
  constexpr auto Kg = 0.2708_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  CalculateAndSimulate(armFF, Ks, Kv, Ka, Kg, 1_rad, 0.02_rad_per_s,
                       0_rad_per_s, 20_ms);
}

TEST(ArmFeedforwardTest, AchievableVelocity) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  EXPECT_NEAR(armFF
                  .MaxAchievableVelocity(12_V, std::numbers::pi / 3 * 1_rad,
                                         1_rad_per_s_sq)
                  .value(),
              6, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableVelocity(11.5_V, std::numbers::pi / 3 * 1_rad,
                                         1_rad_per_s_sq)
                  .value(),
              -9, 0.002);
}

TEST(ArmFeedforwardTest, AchievableAcceleration) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  EXPECT_NEAR(armFF
                  .MaxAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                             1_rad_per_s)
                  .value(),
              4.75, 0.002);
  EXPECT_NEAR(armFF
                  .MaxAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                             -1_rad_per_s)
                  .value(),
              6.75, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                             1_rad_per_s)
                  .value(),
              -7.25, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                             -1_rad_per_s)
                  .value(),
              -5.25, 0.002);
}

TEST(ArmFeedforwardTest, NegativeGains) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;
  wpi::math::ArmFeedforward armFF{Ks, Kg, -Kv, -Ka};

  EXPECT_EQ(armFF.GetKv().value(), 0);
  EXPECT_EQ(armFF.GetKa().value(), 0);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>

#include <gtest/gtest.h>

#include "frc/EigenCore.h"
#include "frc/controller/ArmFeedforward.h"
#include "frc/system/NumericalIntegration.h"
#include "units/angular_acceleration.h"
#include "units/angular_velocity.h"
#include "units/time.h"
#include "units/voltage.h"

static constexpr auto Ks = 0.5_V;
static constexpr auto Kv = 1.5_V / 1_rad_per_s;
static constexpr auto Ka = 2_V / 1_rad_per_s_sq;
static constexpr auto Kg = 1_V;

namespace {

/**
 * Simulates a single-jointed arm and returns the final state.
 *
 * @param currentAngle The starting angle.
 * @param currentVelocity The starting angular velocity.
 * @param input The input voltage.
 * @param dt The simulation time.
 * @return The final state as a 2-vector of angle and angular velocity.
 */
frc::Matrixd<2, 1> Simulate(units::radian_t currentAngle,
                            units::radians_per_second_t currentVelocity,
                            units::volt_t input, units::second_t dt) {
  constexpr frc::Matrixd<2, 2> A{{0.0, 1.0}, {0.0, -Kv.value() / Ka.value()}};
  constexpr frc::Matrixd<2, 1> B{{0.0}, {1.0 / Ka.value()}};

  return frc::RK4(
      [&](const frc::Matrixd<2, 1>& x,
          const frc::Matrixd<1, 1>& u) -> frc::Matrixd<2, 1> {
        frc::Matrixd<2, 1> c{0.0, -Ks.value() / Ka.value() * wpi::sgn(x(1)) -
                                      Kg.value() / Ka.value() * std::cos(x(0))};
        return A * x + B * u + c;
      },
      frc::Matrixd<2, 1>{currentAngle.value(), currentVelocity.value()},
      frc::Matrixd<1, 1>{input.value()}, dt);
}

/**
 * Simulates a single-jointed arm and returns the final state.
 *
 * @param armFF The feedforward object.
 * @param currentAngle The starting angle.
 * @param currentVelocity The starting angular velocity.
 * @param input The input voltage.
 * @param dt The simulation time.
 */
void CalculateAndSimulate(const frc::ArmFeedforward& armFF,
                          units::radian_t currentAngle,
                          units::radians_per_second_t currentVelocity,
                          units::radians_per_second_t nextVelocity,
                          units::second_t dt) {
  auto input = armFF.Calculate(currentAngle, currentVelocity, nextVelocity);
  EXPECT_NEAR(nextVelocity.value(),
              Simulate(currentAngle, currentVelocity, input, dt)(1), 1e-12);
}

}  // namespace

TEST(ArmFeedforwardTest, Calculate) {
  frc::ArmFeedforward armFF{Ks, Kg, Kv, Ka};

  // Calculate(angle, angular velocity)
  EXPECT_NEAR(
      armFF.Calculate(std::numbers::pi / 3 * 1_rad, 0_rad_per_s).value(), 0.5,
      0.002);
  EXPECT_NEAR(
      armFF.Calculate(std::numbers::pi / 3 * 1_rad, 1_rad_per_s).value(), 2.5,
      0.002);

  // Calculate(currentAngle, currentVelocity, nextAngle, dt)
  CalculateAndSimulate(armFF, std::numbers::pi / 3 * 1_rad, 1_rad_per_s,
                       1.05_rad_per_s, 20_ms);
  CalculateAndSimulate(armFF, std::numbers::pi / 3 * 1_rad, 1_rad_per_s,
                       0.95_rad_per_s, 20_ms);
  CalculateAndSimulate(armFF, -std::numbers::pi / 3 * 1_rad, 1_rad_per_s,
                       1.05_rad_per_s, 20_ms);
  CalculateAndSimulate(armFF, -std::numbers::pi / 3 * 1_rad, 1_rad_per_s,
                       0.95_rad_per_s, 20_ms);
}

TEST(ArmFeedforwardTest, AchievableVelocity) {
  frc::ArmFeedforward armFF{Ks, Kg, Kv, Ka};
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
  frc::ArmFeedforward armFF{Ks, Kg, Kv, Ka};
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
  frc::ArmFeedforward armFF{Ks, Kg, -Kv, -Ka};
  EXPECT_EQ(armFF.GetKv().value(), 0);
  EXPECT_EQ(armFF.GetKa().value(), 0);
}

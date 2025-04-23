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
template <class Input>
  requires(units::current_unit<Input> || units::voltage_unit<Input>)
frc::Matrixd<2, 1> Simulate(const frc::ArmFeedforward<Input>& armFF,
                            units::radian_t currentAngle,
                            units::radians_per_second_t currentVelocity,
                            units::unit_t<Input> input, units::second_t dt) {
  const frc::Matrixd<2, 2> A{
      {0.0, 1.0}, {0.0, -armFF.GetKv().value() / armFF.GetKa().value()}};
  const frc::Matrixd<2, 1> B{{0.0}, {1.0 / armFF.GetKa().value()}};

  return frc::RK4(
      [&](const frc::Matrixd<2, 1>& x,
          const frc::Matrixd<1, 1>& u) -> frc::Matrixd<2, 1> {
        frc::Matrixd<2, 1> c{
            0.0,
            -armFF.GetKs().value() / armFF.GetKa().value() * wpi::sgn(x(1)) -
                armFF.GetKg().value() / armFF.GetKa().value() * std::cos(x(0))};
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
template <class Input>
  requires(units::current_unit<Input> || units::voltage_unit<Input>)
void CalculateAndSimulate(const frc::ArmFeedforward<Input>& armFF,
                          units::radian_t currentAngle,
                          units::radians_per_second_t currentVelocity,
                          units::radians_per_second_t nextVelocity,
                          units::second_t dt) {
  auto input = armFF.Calculate(currentAngle, currentVelocity, nextVelocity);
  EXPECT_NEAR(nextVelocity.value(),
              Simulate(armFF, currentAngle, currentVelocity, input, dt)(1),
              1e-12);
}

}  // namespace

TEST(ArmFeedforwardTest, CalculateVoltage) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;

  frc::ArmFeedforward<units::volt> armFF{Ks, Kg, Kv, Ka};

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

TEST(ArmFeedforwardTest, CalculateCurrent) {
  constexpr auto Ks = 0.5_A;
  constexpr auto Kv = 1.5_A / 1_rad_per_s;
  constexpr auto Ka = 2_A / 1_rad_per_s_sq;
  constexpr auto Kg = 1_A;

  frc::ArmFeedforward<units::ampere> armFF{Ks, Kg, Kv, Ka};

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

TEST(ArmFeedforwardTest, AchievableVelocityVoltage) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;

  frc::ArmFeedforward<units::volt> armFF{Ks, Kg, Kv, Ka};
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

TEST(ArmFeedforwardTest, AchievableVelocityCurrent) {
  constexpr auto Ks = 0.5_A;
  constexpr auto Kv = 1.5_A / 1_rad_per_s;
  constexpr auto Ka = 2_A / 1_rad_per_s_sq;
  constexpr auto Kg = 1_A;

  frc::ArmFeedforward<units::ampere> armFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(armFF
                  .MaxAchievableVelocity(12_A, std::numbers::pi / 3 * 1_rad,
                                         1_rad_per_s_sq)
                  .value(),
              6, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableVelocity(11.5_A, std::numbers::pi / 3 * 1_rad,
                                         1_rad_per_s_sq)
                  .value(),
              -9, 0.002);
}

TEST(ArmFeedforwardTest, AchievableAccelerationVoltage) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;

  frc::ArmFeedforward<units::volt> armFF{Ks, Kg, Kv, Ka};
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

TEST(ArmFeedforwardTest, AchievableAccelerationCurrent) {
  constexpr auto Ks = 0.5_A;
  constexpr auto Kv = 1.5_A / 1_rad_per_s;
  constexpr auto Ka = 2_A / 1_rad_per_s_sq;
  constexpr auto Kg = 1_A;

  frc::ArmFeedforward<units::ampere> armFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(armFF
                  .MaxAchievableAcceleration(12_A, std::numbers::pi / 3 * 1_rad,
                                             1_rad_per_s)
                  .value(),
              4.75, 0.002);
  EXPECT_NEAR(armFF
                  .MaxAchievableAcceleration(12_A, std::numbers::pi / 3 * 1_rad,
                                             -1_rad_per_s)
                  .value(),
              6.75, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableAcceleration(12_A, std::numbers::pi / 3 * 1_rad,
                                             1_rad_per_s)
                  .value(),
              -7.25, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableAcceleration(12_A, std::numbers::pi / 3 * 1_rad,
                                             -1_rad_per_s)
                  .value(),
              -5.25, 0.002);
}

TEST(ArmFeedforwardTest, NegativeGainsVoltage) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V / 1_rad_per_s;
  constexpr auto Ka = 2_V / 1_rad_per_s_sq;
  constexpr auto Kg = 1_V;

  frc::ArmFeedforward<units::volt> armFF{Ks, Kg, -Kv, -Ka};
  EXPECT_EQ(armFF.GetKv().value(), 0);
  EXPECT_EQ(armFF.GetKa().value(), 0);
}

TEST(ArmFeedforwardTest, NegativeGainsCurrent) {
  constexpr auto Ks = 0.5_A;
  constexpr auto Kv = 1.5_A / 1_rad_per_s;
  constexpr auto Ka = 2_A / 1_rad_per_s_sq;
  constexpr auto Kg = 1_A;

  frc::ArmFeedforward<units::ampere> armFF{Ks, Kg, -Kv, -Ka};
  EXPECT_EQ(armFF.GetKv().value(), 0);
  EXPECT_EQ(armFF.GetKa().value(), 0);
}

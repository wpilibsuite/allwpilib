// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <gtest/gtest.h>

#include "wpi/math/controller/ProfiledPIDController.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"

TEST(ProfiledPIDInputOutputTest, ContinuousInput1) {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(-180_deg, 180_deg);

  static constexpr wpi::units::degree_t kSetpoint{-179.0};
  static constexpr wpi::units::degree_t kMeasurement{-179.0};
  static constexpr wpi::units::degree_t kGoal{179.0};

  controller.Reset(kSetpoint);
  EXPECT_LT(controller.Calculate(kMeasurement, kGoal), 0.0_deg);

  // Error must be less than half the input range at all times
  EXPECT_LT(
      wpi::units::math::abs(controller.GetSetpoint().position - kMeasurement),
      180_deg);
}

TEST(ProfiledPIDInputOutputTest, ContinuousInput2) {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(-wpi::units::radian_t{std::numbers::pi},
                                   wpi::units::radian_t{std::numbers::pi});

  static constexpr wpi::units::radian_t kSetpoint{-3.4826633343199735};
  static constexpr wpi::units::radian_t kMeasurement{-3.1352207333939606};
  static constexpr wpi::units::radian_t kGoal{-3.534162788601621};

  controller.Reset(kSetpoint);
  EXPECT_LT(controller.Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(
      wpi::units::math::abs(controller.GetSetpoint().position - kMeasurement),
      wpi::units::radian_t{std::numbers::pi});
}

TEST(ProfiledPIDInputOutputTest, ContinuousInput3) {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(-wpi::units::radian_t{std::numbers::pi},
                                   wpi::units::radian_t{std::numbers::pi});

  static constexpr wpi::units::radian_t kSetpoint{-3.5176604690006377};
  static constexpr wpi::units::radian_t kMeasurement{3.1191729343822456};
  static constexpr wpi::units::radian_t kGoal{2.709680418117445};

  controller.Reset(kSetpoint);
  EXPECT_LT(controller.Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(
      wpi::units::math::abs(controller.GetSetpoint().position - kMeasurement),
      wpi::units::radian_t{std::numbers::pi});
}

TEST(ProfiledPIDInputOutputTest, ContinuousInput4) {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(
      0_rad, wpi::units::radian_t{2.0 * std::numbers::pi});

  static constexpr wpi::units::radian_t kSetpoint{2.78};
  static constexpr wpi::units::radian_t kMeasurement{3.12};
  static constexpr wpi::units::radian_t kGoal{2.71};

  controller.Reset(kSetpoint);
  EXPECT_LT(controller.Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(
      wpi::units::math::abs(controller.GetSetpoint().position - kMeasurement),
      wpi::units::radian_t{std::numbers::pi});
}

TEST(ProfiledPIDInputOutputTest, ProportionalGainOutput) {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(4);

  EXPECT_DOUBLE_EQ(-0.1, controller.Calculate(0.025_deg, 0_deg));
}

TEST(ProfiledPIDInputOutputTest, IntegralGainOutput) {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetI(4);

  double out = 0;

  for (int i = 0; i < 5; i++) {
    out = controller.Calculate(0.025_deg, 0_deg);
  }

  EXPECT_DOUBLE_EQ(-0.5 * controller.GetPeriod().value(), out);
}

TEST(ProfiledPIDInputOutputTest, DerivativeGainOutput) {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetD(4);

  controller.Calculate(0_deg, 0_deg);

  EXPECT_DOUBLE_EQ(-10_ms / controller.GetPeriod(),
                   controller.Calculate(0.0025_deg, 0_deg));
}

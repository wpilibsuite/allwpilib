// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <gtest/gtest.h>

#include "frc/controller/ExponentialPIDController.h"
#include "units/angle.h"
#include "units/angular_acceleration.h"
#include "units/angular_velocity.h"
#include "units/voltage.h"

TEST(ExponentialPIDInputOutputTest, ContinuousInput1) {
  frc::ExponentialPIDController<units::degree,units::volt> controller{
      0.0, 0.0, 0.0, {12_V, 1, 1}};

  controller.SetP(1);
  controller.EnableContinuousInput(-180_deg, 180_deg);

  static constexpr units::degree_t kSetpoint{-179.0};
  static constexpr units::degree_t kMeasurement{-179.0};
  static constexpr units::degree_t kGoal{179.0};

  controller.Reset(kSetpoint);
  EXPECT_LT(controller.Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(units::math::abs(controller.GetSetpoint().position - kMeasurement),
            180_deg);
}

TEST(ExponentialPIDInputOutputTest, ContinuousInput2) {
  frc::ExponentialPIDController<units::degree,units::volt> controller{
      0.0, 0.0, 0.0, {12_V, 1, 1}};

  controller.SetP(1);
  controller.EnableContinuousInput(-units::radian_t{std::numbers::pi},
                                   units::radian_t{std::numbers::pi});

  static constexpr units::radian_t kSetpoint{-3.4826633343199735};
  static constexpr units::radian_t kMeasurement{-3.1352207333939606};
  static constexpr units::radian_t kGoal{-3.534162788601621};

  controller.Reset(kSetpoint);
  EXPECT_LT(controller.Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(units::math::abs(controller.GetSetpoint().position - kMeasurement),
            units::radian_t{std::numbers::pi});
}

TEST(ExponentialPIDInputOutputTest, ContinuousInput3) {
  frc::ExponentialPIDController<units::degree,units::volt> controller{
      0.0, 0.0, 0.0, {12_V, 1, 1}};

  controller.SetP(1);
  controller.EnableContinuousInput(-units::radian_t{std::numbers::pi},
                                   units::radian_t{std::numbers::pi});

  static constexpr units::radian_t kSetpoint{-3.5176604690006377};
  static constexpr units::radian_t kMeasurement{3.1191729343822456};
  static constexpr units::radian_t kGoal{2.709680418117445};

  controller.Reset(kSetpoint);
  EXPECT_LT(controller.Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(units::math::abs(controller.GetSetpoint().position - kMeasurement),
            units::radian_t{std::numbers::pi});
}

TEST(ExponentialPIDInputOutputTest, ContinuousInput4) {
  frc::ExponentialPIDController<units::degree,units::volt> controller{
      0.0, 0.0, 0.0, {12_V, 1, 1}};

  controller.SetP(1);
  controller.EnableContinuousInput(0_rad,
                                   units::radian_t{2.0 * std::numbers::pi});

  static constexpr units::radian_t kSetpoint{2.78};
  static constexpr units::radian_t kMeasurement{3.12};
  static constexpr units::radian_t kGoal{2.71};

  controller.Reset(kSetpoint);
  EXPECT_LT(controller.Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(units::math::abs(controller.GetSetpoint().position - kMeasurement),
            units::radian_t{std::numbers::pi});
}

TEST(ExponentialPIDInputOutputTest, ContinuousInput5) {
  frc::ExponentialPIDController<units::degree,units::volt> controller{
      0.0, 0.0, 0.0, {12_V, 1, 1}};

  controller.SetP(4);

  EXPECT_DOUBLE_EQ(-0.1, controller.Calculate(0.025_deg, 0_deg));
}

TEST(ExponentialPIDInputOutputTest, IntegralGainOutput) {
  frc::ExponentialPIDController<units::degree,units::volt> controller{
      0.0, 0.0, 0.0, {12_V, 1, 1}};

  controller.SetI(4);

  double out = 0;

  for (int i = 0; i < 5; i++) {
    out = controller.Calculate(0.025_deg, 0_deg);
  }

  EXPECT_DOUBLE_EQ(-0.5 * controller.GetPeriod().value(), out);
}

TEST(ExponentialPIDInputOutputTest, DerivativeGainOutput) {
  frc::ExponentialPIDController<units::degree,units::volt> controller{
      0.0, 0.0, 0.0, {12_V, 1, 1}};

  controller.SetD(4);

  controller.Calculate(0_deg, 0_deg);

  EXPECT_DOUBLE_EQ(-10_ms / controller.GetPeriod(),
                   controller.Calculate(0.0025_deg, 0_deg));
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <units/angle.h>
#include <units/angular_acceleration.h>
#include <units/angular_velocity.h>
#include <wpi/math>

#include "frc/controller/ProfiledPIDController.h"
#include "gtest/gtest.h"

class ProfiledPIDInputOutputTest : public testing::Test {
 protected:
  frc::ProfiledPIDController<units::degrees>* controller;

  void SetUp() override {
    controller = new frc::ProfiledPIDController<units::degrees>(
        0, 0, 0, {360_deg_per_s, 180_deg_per_s_sq});
  }

  void TearDown() override { delete controller; }
};

TEST_F(ProfiledPIDInputOutputTest, ContinuousInputTest1) {
  controller->SetP(1);
  controller->EnableContinuousInput(-180_deg, 180_deg);

  static constexpr units::degree_t kSetpoint{-179.0};
  static constexpr units::degree_t kMeasurement{-179.0};
  static constexpr units::degree_t kGoal{179.0};

  controller->Reset(kSetpoint);
  EXPECT_LT(controller->Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(units::math::abs(controller->GetSetpoint().position - kMeasurement),
            180_deg);
}

TEST_F(ProfiledPIDInputOutputTest, ContinuousInputTest2) {
  controller->SetP(1);
  controller->EnableContinuousInput(-units::radian_t{wpi::math::pi},
                                    units::radian_t{wpi::math::pi});

  static constexpr units::radian_t kSetpoint{-3.4826633343199735};
  static constexpr units::radian_t kMeasurement{-3.1352207333939606};
  static constexpr units::radian_t kGoal{-3.534162788601621};

  controller->Reset(kSetpoint);
  EXPECT_LT(controller->Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(units::math::abs(controller->GetSetpoint().position - kMeasurement),
            units::radian_t{wpi::math::pi});
}

TEST_F(ProfiledPIDInputOutputTest, ContinuousInputTest3) {
  controller->SetP(1);
  controller->EnableContinuousInput(-units::radian_t{wpi::math::pi},
                                    units::radian_t{wpi::math::pi});

  static constexpr units::radian_t kSetpoint{-3.5176604690006377};
  static constexpr units::radian_t kMeasurement{3.1191729343822456};
  static constexpr units::radian_t kGoal{2.709680418117445};

  controller->Reset(kSetpoint);
  EXPECT_LT(controller->Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(units::math::abs(controller->GetSetpoint().position - kMeasurement),
            units::radian_t{wpi::math::pi});
}

TEST_F(ProfiledPIDInputOutputTest, ContinuousInputTest4) {
  controller->SetP(1);
  controller->EnableContinuousInput(0_rad,
                                    units::radian_t{2.0 * wpi::math::pi});

  static constexpr units::radian_t kSetpoint{2.78};
  static constexpr units::radian_t kMeasurement{3.12};
  static constexpr units::radian_t kGoal{2.71};

  controller->Reset(kSetpoint);
  EXPECT_LT(controller->Calculate(kMeasurement, kGoal), 0.0);

  // Error must be less than half the input range at all times
  EXPECT_LT(units::math::abs(controller->GetSetpoint().position - kMeasurement),
            units::radian_t{wpi::math::pi});
}

TEST_F(ProfiledPIDInputOutputTest, ProportionalGainOutputTest) {
  controller->SetP(4);

  EXPECT_DOUBLE_EQ(-0.1, controller->Calculate(0.025_deg, 0_deg));
}

TEST_F(ProfiledPIDInputOutputTest, IntegralGainOutputTest) {
  controller->SetI(4);

  double out = 0;

  for (int i = 0; i < 5; i++) {
    out = controller->Calculate(0.025_deg, 0_deg);
  }

  EXPECT_DOUBLE_EQ(-0.5 * controller->GetPeriod().to<double>(), out);
}

TEST_F(ProfiledPIDInputOutputTest, DerivativeGainOutputTest) {
  controller->SetD(4);

  controller->Calculate(0_deg, 0_deg);

  EXPECT_DOUBLE_EQ(-10_ms / controller->GetPeriod(),
                   controller->Calculate(0.0025_deg, 0_deg));
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/PIDController.h"
#include "gtest/gtest.h"

class PIDInputOutputTest : public testing::Test {
 protected:
  frc2::PIDController* controller;

  void SetUp() override { controller = new frc2::PIDController{0, 0, 0}; }

  void TearDown() override { delete controller; }
};

TEST_F(PIDInputOutputTest, ContinuousInput) {
  controller->SetP(1);
  controller->EnableContinuousInput(-180, 180);
  EXPECT_DOUBLE_EQ(controller->Calculate(-179, 179), -2);

  controller->EnableContinuousInput(0, 360);
  EXPECT_DOUBLE_EQ(controller->Calculate(1, 359), -2);
}

TEST_F(PIDInputOutputTest, ProportionalGainOutput) {
  controller->SetP(4);

  EXPECT_DOUBLE_EQ(-0.1, controller->Calculate(0.025, 0));
}

TEST_F(PIDInputOutputTest, IntegralGainOutput) {
  controller->SetI(4);

  double out = 0;

  for (int i = 0; i < 5; i++) {
    out = controller->Calculate(0.025, 0);
  }

  EXPECT_DOUBLE_EQ(-0.5 * controller->GetPeriod().value(), out);
}

TEST_F(PIDInputOutputTest, DerivativeGainOutput) {
  controller->SetD(4);

  controller->Calculate(0, 0);

  EXPECT_DOUBLE_EQ(-10_ms / controller->GetPeriod(),
                   controller->Calculate(0.0025, 0));
}

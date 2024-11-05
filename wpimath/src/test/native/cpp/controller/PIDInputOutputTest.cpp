// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/controller/PIDController.h"

TEST(PIDInputOutputTest, ContinuousInput) {
  frc::PIDController controller{0.0, 0.0, 0.0};

  controller.SetP(1);
  controller.EnableContinuousInput(-180, 180);
  EXPECT_DOUBLE_EQ(controller.Calculate(-179, 179), -2);

  controller.EnableContinuousInput(0, 360);
  EXPECT_DOUBLE_EQ(controller.Calculate(1, 359), -2);
}

TEST(PIDInputOutputTest, ProportionalGainOutput) {
  frc::PIDController controller{0.0, 0.0, 0.0};

  controller.SetP(4);

  EXPECT_DOUBLE_EQ(-0.1, controller.Calculate(0.025, 0));
}

TEST(PIDInputOutputTest, IntegralGainOutput) {
  frc::PIDController controller{0.0, 0.0, 0.0};

  controller.SetI(4);

  double out = 0;

  for (int i = 0; i < 5; i++) {
    out = controller.Calculate(0.025, 0);
  }

  EXPECT_DOUBLE_EQ(-0.5 * controller.GetPeriod().value(), out);
}

TEST(PIDInputOutputTest, DerivativeGainOutput) {
  frc::PIDController controller{0.0, 0.0, 0.0};

  controller.SetD(4);

  controller.Calculate(0, 0);

  EXPECT_DOUBLE_EQ(-10_ms / controller.GetPeriod(),
                   controller.Calculate(0.0025, 0));
}

TEST(PIDInputOutputTest, IZoneNoOutput) {
  frc::PIDController controller{0.0, 0.0, 0.0};

  controller.SetI(1);
  controller.SetIZone(1);

  double out = controller.Calculate(2, 0);

  EXPECT_DOUBLE_EQ(0, out);
}

TEST(PIDInputOutputTest, IZoneOutput) {
  frc::PIDController controller{0.0, 0.0, 0.0};

  controller.SetI(1);
  controller.SetIZone(1);

  double out = controller.Calculate(1, 0);

  EXPECT_DOUBLE_EQ(-1 * controller.GetPeriod().value(), out);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/controller/PIDController.h"

static constexpr double kSetpoint = 50.0;
static constexpr double kRange = 200;
static constexpr double kTolerance = 10.0;

TEST(PIDToleranceTest, InitialTolerance) {
  frc::PIDController controller{0.5, 0.0, 0.0};
  controller.EnableContinuousInput(-kRange / 2, kRange / 2);

  EXPECT_FALSE(controller.AtSetpoint());
}

TEST(PIDToleranceTest, AbsoluteTolerance) {
  frc::PIDController controller{0.5, 0.0, 0.0};
  controller.EnableContinuousInput(-kRange / 2, kRange / 2);

  EXPECT_FALSE(controller.AtSetpoint());

  controller.SetTolerance(kTolerance);
  controller.SetSetpoint(kSetpoint);

  EXPECT_FALSE(controller.AtSetpoint())
      << "Error was in tolerance when it should not have been. Error was "
      << controller.GetError();

  controller.Calculate(0.0);

  EXPECT_FALSE(controller.AtSetpoint())
      << "Error was in tolerance when it should not have been. Error was "
      << controller.GetError();

  controller.Calculate(kSetpoint + kTolerance / 2);

  EXPECT_TRUE(controller.AtSetpoint())
      << "Error was not in tolerance when it should have been. Error was "
      << controller.GetError();

  controller.Calculate(kSetpoint + 10 * kTolerance);

  EXPECT_FALSE(controller.AtSetpoint())
      << "Error was in tolerance when it should not have been. Error was "
      << controller.GetError();
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/controller/BangBangController.h"

TEST(BangBangToleranceTest, InTolerance) {
  frc::BangBangController controller{0.1};

  controller.SetSetpoint(1);
  controller.Calculate(1);
  EXPECT_TRUE(controller.AtSetpoint());
}

TEST(BangBangToleranceTest, OutOfTolerance) {
  frc::BangBangController controller{0.1};

  controller.SetSetpoint(1);
  controller.Calculate(0);
  EXPECT_FALSE(controller.AtSetpoint());
}

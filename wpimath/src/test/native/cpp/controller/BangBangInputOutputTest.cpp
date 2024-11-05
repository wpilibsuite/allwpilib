// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/controller/BangBangController.h"

TEST(BangBangInputOutputTest, ShouldOutput) {
  frc::BangBangController controller;

  EXPECT_DOUBLE_EQ(controller.Calculate(0, 1), 1);
}

TEST(BangBangInputOutputTest, ShouldNotOutput) {
  frc::BangBangController controller;

  EXPECT_DOUBLE_EQ(controller.Calculate(1, 0), 0);
}

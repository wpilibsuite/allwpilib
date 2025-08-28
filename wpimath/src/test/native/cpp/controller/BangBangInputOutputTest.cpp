// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpimath/controller/BangBangController.h"

TEST(BangBangInputOutputTest, ShouldOutput) {
  wpimath::BangBangController controller;

  EXPECT_DOUBLE_EQ(controller.Calculate(0, 1), 1);
}

TEST(BangBangInputOutputTest, ShouldNotOutput) {
  wpimath::BangBangController controller;

  EXPECT_DOUBLE_EQ(controller.Calculate(1, 0), 0);
}

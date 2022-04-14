// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/BangBangController.h"
#include "gtest/gtest.h"

class BangBangInputOutputTest : public testing::Test {
 protected:
  frc::BangBangController controller;
};

TEST_F(BangBangInputOutputTest, ShouldOutput) {
  EXPECT_DOUBLE_EQ(controller.Calculate(0, 1), 1);
}

TEST_F(BangBangInputOutputTest, ShouldNotOutput) {
  EXPECT_DOUBLE_EQ(controller.Calculate(1, 0), 0);
}

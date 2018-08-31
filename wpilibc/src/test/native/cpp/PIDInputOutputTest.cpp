/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/PIDController.h"
#include "gtest/gtest.h"

class PIDInputOutputTest : public testing::Test {
 protected:
  frc::experimental::PIDController* controller;

  void SetUp() override {
    controller = new frc::experimental::PIDController(0, 0, 0);
  }

  void TearDown() override { delete controller; }
};

TEST_F(PIDInputOutputTest, OutputRangeTest) {
  controller->SetP(1);
  controller->SetOutputRange(-50, 50);

  EXPECT_DOUBLE_EQ(-50, controller->Calculate(100, 0));
  EXPECT_DOUBLE_EQ(50, controller->Calculate(0, 100));
}

TEST_F(PIDInputOutputTest, InputRangeTest) {
  controller->SetP(1);
  controller->SetOutputRange(-1000, 1000);
  controller->SetInputRange(-50, 50);

  EXPECT_DOUBLE_EQ(-100, controller->Calculate(100, 0));
  EXPECT_DOUBLE_EQ(50, controller->Calculate(0, 100));
}

TEST_F(PIDInputOutputTest, ContinuousInputTest) {
  controller->SetP(1);
  controller->SetInputRange(-180, 180);
  controller->SetContinuous(true);

  EXPECT_TRUE(controller->Calculate(-179, 179) < 0);
}

TEST_F(PIDInputOutputTest, ProportionalGainOutputTest) {
  controller->SetP(4);

  EXPECT_DOUBLE_EQ(-.1, controller->Calculate(.025, 0));
}

TEST_F(PIDInputOutputTest, IntegralGainOutputTest) {
  controller->SetI(4);

  double out = 0;

  for (int i = 0; i < 5; i++) {
    out = controller->Calculate(.025, 0);
  }

  EXPECT_DOUBLE_EQ(-.5 * controller->GetPeriod(), out);
}

TEST_F(PIDInputOutputTest, DerivativeGainOutputTest) {
  controller->SetD(4);

  controller->Calculate(0, 0);

  EXPECT_DOUBLE_EQ(-.01 / controller->GetPeriod(),
                   controller->Calculate(.0025, 0));
}

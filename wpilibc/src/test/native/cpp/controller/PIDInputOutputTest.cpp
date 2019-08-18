/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/PIDController.h"
#include "gtest/gtest.h"

using units::meter_t;

class PIDInputOutputTest : public testing::Test {
 protected:
  frc2::PIDController<meter_t>* controller;

  void SetUp() override {
    controller = new frc2::PIDController<meter_t>(0, 0, 0);
  }

  void TearDown() override { delete controller; }
};

TEST_F(PIDInputOutputTest, OutputRangeTest) {
  controller->SetP(1);
  controller->SetOutputRange(-50, 50);

  EXPECT_DOUBLE_EQ(-50, controller->Calculate(100_m, 0_m));
  EXPECT_DOUBLE_EQ(50, controller->Calculate(0_m, 100_m));
}

TEST_F(PIDInputOutputTest, ContinuousInputTest) {
  controller->SetP(1);
  controller->EnableContinuousInput(-180_m, 180_m);

  EXPECT_TRUE(controller->Calculate(-179_m, 179_m) < 0);
}

TEST_F(PIDInputOutputTest, ProportionalGainOutputTest) {
  controller->SetP(4);

  EXPECT_DOUBLE_EQ(-0.1, controller->Calculate(0.025_m, 0_m));
}

TEST_F(PIDInputOutputTest, IntegralGainOutputTest) {
  controller->SetI(4);

  double out = 0;

  for (int i = 0; i < 5; i++) {
    out = controller->Calculate(0.025_m, 0_m);
  }

  EXPECT_DOUBLE_EQ(-0.5 * controller->GetPeriod().to<double>(), out);
}

TEST_F(PIDInputOutputTest, DerivativeGainOutputTest) {
  controller->SetD(4);

  controller->Calculate(0_m, 0_m);

  EXPECT_DOUBLE_EQ(-0.01_s / controller->GetPeriod(),
                   controller->Calculate(0.0025_m, 0_m));
}

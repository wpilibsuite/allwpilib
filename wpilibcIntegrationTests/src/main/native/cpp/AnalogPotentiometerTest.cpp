// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogPotentiometer.h"  // NOLINT(build/include_order)

#include "TestBench.h"
#include "frc/AnalogOutput.h"
#include "frc/RobotController.h"
#include "frc/Timer.h"
#include "gtest/gtest.h"

using namespace frc;

static const double kScale = 270.0;
static const double kAngle = 180.0;

class AnalogPotentiometerTest : public testing::Test {
 protected:
  AnalogOutput* m_fakePot;
  AnalogPotentiometer* m_pot;

  void SetUp() override {
    m_fakePot = new AnalogOutput(TestBench::kAnalogOutputChannel);
    m_pot =
        new AnalogPotentiometer(TestBench::kFakeAnalogOutputChannel, kScale);
  }

  void TearDown() override {
    delete m_fakePot;
    delete m_pot;
  }
};

TEST_F(AnalogPotentiometerTest, TestInitialSettings) {
  m_fakePot->SetVoltage(0.0);
  Wait(0.1);
  EXPECT_NEAR(0.0, m_pot->Get(), 5.0)
      << "The potentiometer did not initialize to 0.";
}

TEST_F(AnalogPotentiometerTest, TestRangeValues) {
  m_fakePot->SetVoltage(kAngle / kScale * RobotController::GetVoltage5V());
  Wait(0.1);
  EXPECT_NEAR(kAngle, m_pot->Get(), 2.0)
      << "The potentiometer did not measure the correct angle.";
}

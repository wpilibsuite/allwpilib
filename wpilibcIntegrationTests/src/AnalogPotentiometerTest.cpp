/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <AnalogOutput.h>
#include <AnalogPotentiometer.h>
#include <Timer.h>
#include <ControllerPower.h>
#include "TestBench.h"
#include "gtest/gtest.h"

static const double kScale = 270.0;
static const double kAngle = 180.0;

class AnalogPotentiometerTest : public testing::Test {
 protected:
  AnalogOutput *m_fakePot;
  AnalogPotentiometer *m_pot;

  virtual void SetUp() override {
    m_fakePot = new AnalogOutput(TestBench::kAnalogOutputChannel);
    m_pot =
        new AnalogPotentiometer(TestBench::kFakeAnalogOutputChannel, kScale);
  }

  virtual void TearDown() override {
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
  m_fakePot->SetVoltage(kAngle / kScale * ControllerPower::GetVoltage5V());
  Wait(0.1);
  EXPECT_NEAR(kAngle, m_pot->Get(), 2.0)
      << "The potentiometer did not measure the correct angle.";
}

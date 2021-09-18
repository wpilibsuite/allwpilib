// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogPotentiometer.h"  // NOLINT(build/include_order)

#include "TestBench.h"
#include "frc/AnalogOutput.h"
#include "frc/RobotController.h"
#include "frc/Timer.h"
#include "gtest/gtest.h"

static constexpr double kScale = 270.0;
static constexpr double kAngle = 180.0;

TEST(AnalogPotentiometerTest, InitialSettings) {
  frc::AnalogOutput m_fakePot{TestBench::kAnalogOutputChannel};
  frc::AnalogPotentiometer m_pot{TestBench::kFakeAnalogOutputChannel, kScale};

  m_fakePot.SetVoltage(0.0);
  frc::Wait(100_ms);
  EXPECT_NEAR(0.0, m_pot.Get(), 5.0)
      << "The potentiometer did not initialize to 0.";
}

TEST(AnalogPotentiometerTest, RangeValues) {
  frc::AnalogOutput m_fakePot{TestBench::kAnalogOutputChannel};
  frc::AnalogPotentiometer m_pot{TestBench::kFakeAnalogOutputChannel, kScale};

  m_fakePot.SetVoltage(kAngle / kScale * frc::RobotController::GetVoltage5V());
  frc::Wait(100_ms);
  EXPECT_NEAR(kAngle, m_pot.Get(), 2.0)
      << "The potentiometer did not measure the correct angle.";
}

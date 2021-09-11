// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PowerDistribution.h"  // NOLINT(build/include_order)

#include <hal/Ports.h>
#include <units/time.h>

#include "TestBench.h"
#include "frc/Timer.h"
#include "frc/motorcontrol/Talon.h"
#include "gtest/gtest.h"

static constexpr auto kMotorTime = 0.25_s;

class PowerDistributionTest : public testing::Test {
 protected:
  frc::PowerDistribution m_pdp;
  frc::Talon m_talon{TestBench::kTalonChannel};
};

TEST_F(PowerDistributionTest, CheckRepeatedCalls) {
  auto numChannels = HAL_GetNumCTREPDPChannels();
  // 1 second
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < numChannels; j++) {
      m_pdp.GetCurrent(j);
    }
    m_pdp.GetVoltage();
  }
  frc::Wait(20_ms);
}

/**
 * Test if the current changes when the motor is driven using a talon
 */
TEST_F(PowerDistributionTest, CheckCurrentTalon) {
  frc::Wait(kMotorTime);

  /* The Current should be 0 */
  EXPECT_FLOAT_EQ(0, m_pdp.GetCurrent(TestBench::kTalonPDPChannel))
      << "The Talon current was non-zero";

  /* Set the motor to full forward */
  m_talon.Set(1.0);
  frc::Wait(kMotorTime);

  /* The current should now be positive */
  ASSERT_GT(m_pdp.GetCurrent(TestBench::kTalonPDPChannel), 0)
      << "The Talon current was not positive";
}

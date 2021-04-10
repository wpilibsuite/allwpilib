// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PowerDistributionPanel.h"  // NOLINT(build/include_order)

#include <thread>

#include <hal/Ports.h>

#include "TestBench.h"
#include "frc/Timer.h"
#include "frc/motorcontrol/Jaguar.h"
#include "frc/motorcontrol/Talon.h"
#include "frc/motorcontrol/Victor.h"
#include "gtest/gtest.h"

using namespace frc;

static const double kMotorTime = 0.25;

class PowerDistributionPanelTest : public testing::Test {
 protected:
  PowerDistributionPanel* m_pdp;
  Talon* m_talon;
  Victor* m_victor;
  Jaguar* m_jaguar;

  void SetUp() override {
    m_pdp = new PowerDistributionPanel();
    m_talon = new Talon(TestBench::kTalonChannel);
    m_victor = new Victor(TestBench::kVictorChannel);
    m_jaguar = new Jaguar(TestBench::kJaguarChannel);
  }

  void TearDown() override {
    delete m_pdp;
    delete m_talon;
    delete m_victor;
    delete m_jaguar;
  }
};

TEST_F(PowerDistributionPanelTest, CheckRepeatedCalls) {
  auto numChannels = HAL_GetNumPDPChannels();
  // 1 second
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < numChannels; j++) {
      m_pdp->GetCurrent(j);
      ASSERT_TRUE(m_pdp->GetError().GetCode() == 0);
    }
    m_pdp->GetVoltage();
    ASSERT_TRUE(m_pdp->GetError().GetCode() == 0);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

/**
 * Test if the current changes when the motor is driven using a talon
 */
TEST_F(PowerDistributionPanelTest, CheckCurrentTalon) {
  Wait(kMotorTime);

  /* The Current should be 0 */
  EXPECT_FLOAT_EQ(0, m_pdp->GetCurrent(TestBench::kTalonPDPChannel))
      << "The Talon current was non-zero";

  /* Set the motor to full forward */
  m_talon->Set(1.0);
  Wait(kMotorTime);

  /* The current should now be positive */
  ASSERT_GT(m_pdp->GetCurrent(TestBench::kTalonPDPChannel), 0)
      << "The Talon current was not positive";
}

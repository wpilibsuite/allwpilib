/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PowerDistributionPanel.h"  // NOLINT(build/include_order)

#include "Jaguar.h"
#include "Talon.h"
#include "TestBench.h"
#include "Timer.h"
#include "Victor.h"
#include "gtest/gtest.h"

static const double kMotorTime = 0.25;

class PowerDistributionPanelTest : public testing::Test {
 protected:
  frc::PowerDistributionPanel* m_pdp;
  frc::Talon* m_talon;
  frc::Victor* m_victor;
  frc::Jaguar* m_jaguar;

  void SetUp() override {
    m_pdp = new frc::PowerDistributionPanel();
    m_talon = new frc::Talon(TestBench::kTalonChannel);
    m_victor = new frc::Victor(TestBench::kVictorChannel);
    m_jaguar = new frc::Jaguar(TestBench::kJaguarChannel);
  }

  void TearDown() override {
    delete m_pdp;
    delete m_talon;
    delete m_victor;
    delete m_jaguar;
  }
};

/**
 * Test if the current changes when the motor is driven using a talon
 */
TEST_F(PowerDistributionPanelTest, CheckCurrentTalon) {
  frc::Wait(kMotorTime);

  /* The Current should be 0 */
  EXPECT_FLOAT_EQ(0, m_pdp->GetCurrent(TestBench::kTalonPDPChannel))
      << "The Talon current was non-zero";

  /* Set the motor to full forward */
  m_talon->Set(1.0);
  frc::Wait(kMotorTime);

  /* The current should now be positive */
  ASSERT_GT(m_pdp->GetCurrent(TestBench::kTalonPDPChannel), 0)
      << "The Talon current was not positive";
}

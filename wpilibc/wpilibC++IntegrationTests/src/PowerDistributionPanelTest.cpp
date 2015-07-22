/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <Jaguar.h>
#include <PowerDistributionPanel.h>
#include <Talon.h>
#include <Timer.h>
#include <Victor.h>
#include "gtest/gtest.h"
#include "TestBench.h"

static const double kMotorTime = 0.25;

class PowerDistributionPanelTest : public testing::Test {
 protected:
  PowerDistributionPanel *m_pdp;
  Talon *m_talon;
  Victor *m_victor;
  Jaguar *m_jaguar;

  virtual void SetUp() override {
    m_pdp = new PowerDistributionPanel();
    m_talon = new Talon(TestBench::kTalonChannel);
    m_victor = new Victor(TestBench::kVictorChannel);
    m_jaguar = new Jaguar(TestBench::kJaguarChannel);
  }

  virtual void TearDown() override {
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

/**
 * Test if the current changes when the motor is driven using a victor
 */
TEST_F(PowerDistributionPanelTest, CheckCurrentVictor) {
  Wait(kMotorTime);

  /* The Current should be 0 */
  EXPECT_FLOAT_EQ(0, m_pdp->GetCurrent(TestBench::kVictorPDPChannel))
      << "The Victor current was non-zero";

  /* Set the motor to full forward */
  m_victor->Set(1.0);
  Wait(kMotorTime);

  /* The current should now be positive */
  ASSERT_GT(m_pdp->GetCurrent(TestBench::kVictorPDPChannel), 0)
      << "The Victor current was not positive";
}

/**
 * Test if the current changes when the motor is driven using a jaguar
 */
TEST_F(PowerDistributionPanelTest, CheckCurrentJaguar) {
  Wait(kMotorTime);

  /* The Current should be 0 */
  EXPECT_FLOAT_EQ(0, m_pdp->GetCurrent(TestBench::kJaguarPDPChannel))
      << "The Jaguar current was non-zero";

  /* Set the motor to full forward */
  m_jaguar->Set(1.0);
  Wait(kMotorTime);

  /* The current should now be positive */
  ASSERT_GT(m_pdp->GetCurrent(TestBench::kJaguarPDPChannel), 0)
      << "The Jaguar current was not positive";
}

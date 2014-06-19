/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

/* The current returned when the motor is not being driven */
static const double kLowCurrent =  1.52;
static const double kCurrentTolerance =  0.1;

class PowerDistributionPanelTest : public testing::Test {
protected:
  PowerDistributionPanel *m_pdp;
  Talon *m_talon;
  Victor *m_victor;
  Jaguar *m_jaguar;

  virtual void SetUp() {
    m_pdp = new PowerDistributionPanel();
    m_talon = new Talon(TestBench::kTalonChannel);
    m_victor = new Victor(TestBench::kVictorChannel);
    m_jaguar = new Jaguar(TestBench::kJaguarChannel);
  }

  virtual void TearDown() {
    delete m_pdp;
    delete m_talon;
    delete m_victor;
    delete m_jaguar;
  }

  void Reset() {
    /* Reset all speed controllers to 0.0 */
    m_talon->Set(0.0f);
    m_victor->Set(0.0f);
    m_jaguar->Set(0.0f);
  }
};

/**
 * Test if the current changes when the motor is driven using a talon
 */
TEST_F(PowerDistributionPanelTest, CheckCurrentTalon) {
  Reset();

  /* The Current should be kLowCurrent */
  EXPECT_NEAR(kLowCurrent, m_pdp->GetCurrent(TestBench::kTalonPDPChannel), kCurrentTolerance)
    << "The low current was not within the expected range.";

  /* Set the motor to full forward */
  m_talon->Set(1.0);
  Wait(0.02);

  /* The current should now be greater than the low current */
  ASSERT_GT(m_pdp->GetCurrent(TestBench::kTalonPDPChannel), kLowCurrent)
    << "The driven current is not greater than the resting current.";
}

/**
 * Test if the current changes when the motor is driven using a victor
 */
TEST_F(PowerDistributionPanelTest,CheckCurrentVictor) {
  Reset();

  /* The Current should be kLowCurrent */
  EXPECT_NEAR(kLowCurrent, m_pdp->GetCurrent(TestBench::kVictorPDPChannel), kCurrentTolerance)
    << "The low current was not within the expected range.";

  /* Set the motor to full forward */
  m_victor->Set(1.0);
  Wait(0.02);

  /* The current should now be greater than the low current */
  ASSERT_GT(m_pdp->GetCurrent(TestBench::kVictorPDPChannel), kLowCurrent)
    << "The driven current is not greater than the resting current.";
}

/**
 * Test if the current changes when the motor is driven using a jaguar
 */
TEST_F(PowerDistributionPanelTest, CheckCurrentJaguar) {
  Reset();

  /* The Current should be kLowCurrent */
  EXPECT_NEAR(kLowCurrent, m_pdp->GetCurrent(TestBench::kJaguarPDPChannel), kCurrentTolerance)
    << "The low current was not within the expected range.";

  /* Set the motor to full forward */
  m_jaguar->Set(1.0);
  Wait(0.02);

  /* The current should now be greater than the low current */
  ASSERT_GT(m_pdp->GetCurrent(TestBench::kJaguarPDPChannel), kLowCurrent)
    << "The driven current is not greater than the resting current.";
}

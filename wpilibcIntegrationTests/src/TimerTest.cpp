/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Timer.h"  // NOLINT(build/include_order)

#include "TestBench.h"
#include "gtest/gtest.h"

using namespace frc;

static const double kWaitTime = 0.5;

class TimerTest : public testing::Test {
 protected:
  Timer* m_timer;

  void SetUp() override { m_timer = new Timer; }

  void TearDown() override { delete m_timer; }

  void Reset() { m_timer->Reset(); }
};

/**
 * Test if the Wait function works
 */
TEST_F(TimerTest, Wait) {
  Reset();

  double initialTime = m_timer->GetFPGATimestamp();

  Wait(kWaitTime);

  double finalTime = m_timer->GetFPGATimestamp();

  EXPECT_NEAR(kWaitTime, finalTime - initialTime, 0.001);
}

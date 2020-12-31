// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Timer.h"  // NOLINT(build/include_order)

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

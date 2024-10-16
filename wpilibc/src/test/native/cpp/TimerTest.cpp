// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Timer.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>

#include "frc/simulation/SimHooks.h"

using namespace frc;

namespace {
class TimerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    frc::sim::PauseTiming();
    frc::sim::RestartTiming();
  }

  void TearDown() override { frc::sim::ResumeTiming(); }
};

}  // namespace

TEST_F(TimerTest, StartStop) {
  Timer timer;

  // Verify timer is initialized as stopped
  EXPECT_EQ(timer.Get(), 0_s);
  EXPECT_FALSE(timer.IsRunning());
  frc::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 0_s);
  EXPECT_FALSE(timer.IsRunning());

  // Verify timer increments after it's started
  timer.Start();
  frc::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 500_ms);
  EXPECT_TRUE(timer.IsRunning());

  // Verify timer stops incrementing after it's stopped
  timer.Stop();
  frc::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 500_ms);
  EXPECT_FALSE(timer.IsRunning());
}

TEST_F(TimerTest, Reset) {
  Timer timer;
  timer.Start();

  // Advance timer to 500 ms
  EXPECT_EQ(timer.Get(), 0_s);
  frc::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 500_ms);

  // Verify timer reports 0 ms after reset
  timer.Reset();
  EXPECT_EQ(timer.Get(), 0_s);

  // Verify timer continues incrementing
  frc::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 500_ms);

  // Verify timer doesn't start incrementing after reset if it was stopped
  timer.Stop();
  timer.Reset();
  frc::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 0_ms);
}

TEST_F(TimerTest, HasElapsed) {
  Timer timer;

  // Verify 0 ms has elapsed since timer hasn't started
  EXPECT_TRUE(timer.HasElapsed(0_s));

  // Verify timer doesn't report elapsed time when stopped
  frc::sim::StepTiming(500_ms);
  EXPECT_FALSE(timer.HasElapsed(400_ms));

  timer.Start();

  // Verify timer reports >= 400 ms has elapsed after multiple calls
  frc::sim::StepTiming(500_ms);
  EXPECT_TRUE(timer.HasElapsed(400_ms));
  EXPECT_TRUE(timer.HasElapsed(400_ms));
}

TEST_F(TimerTest, AdvanceIfElapsed) {
  Timer timer;

  // Verify 0 ms has elapsed since timer hasn't started
  EXPECT_TRUE(timer.AdvanceIfElapsed(0_s));

  // Verify timer doesn't report elapsed time when stopped
  frc::sim::StepTiming(500_ms);
  EXPECT_FALSE(timer.AdvanceIfElapsed(400_ms));

  timer.Start();

  // Verify timer reports >= 400 ms has elapsed for only first call
  frc::sim::StepTiming(500_ms);
  EXPECT_TRUE(timer.AdvanceIfElapsed(400_ms));
  EXPECT_FALSE(timer.AdvanceIfElapsed(400_ms));

  // Verify timer reports >= 400 ms has elapsed for two calls
  frc::sim::StepTiming(1_s);
  EXPECT_TRUE(timer.AdvanceIfElapsed(400_ms));
  EXPECT_TRUE(timer.AdvanceIfElapsed(400_ms));
  EXPECT_FALSE(timer.AdvanceIfElapsed(400_ms));
}

TEST_F(TimerTest, GetFPGATimestamp) {
  auto start = frc::Timer::GetFPGATimestamp();
  frc::sim::StepTiming(500_ms);
  auto end = frc::Timer::GetFPGATimestamp();
  EXPECT_EQ(start + 500_ms, end);
}

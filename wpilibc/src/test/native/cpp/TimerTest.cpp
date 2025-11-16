// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Timer.hpp"

#include <gtest/gtest.h>

#include "wpi/simulation/SimHooks.hpp"

using namespace wpi;

namespace {
class TimerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    wpi::sim::PauseTiming();
    wpi::sim::RestartTiming();
  }

  void TearDown() override { wpi::sim::ResumeTiming(); }
};

}  // namespace

TEST_F(TimerTest, StartStop) {
  Timer timer;

  // Verify timer is initialized as stopped
  EXPECT_EQ(timer.Get(), 0_s);
  EXPECT_FALSE(timer.IsRunning());
  wpi::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 0_s);
  EXPECT_FALSE(timer.IsRunning());

  // Verify timer increments after it's started
  timer.Start();
  wpi::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 500_ms);
  EXPECT_TRUE(timer.IsRunning());

  // Verify timer stops incrementing after it's stopped
  timer.Stop();
  wpi::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 500_ms);
  EXPECT_FALSE(timer.IsRunning());
}

TEST_F(TimerTest, Reset) {
  Timer timer;
  timer.Start();

  // Advance timer to 500 ms
  EXPECT_EQ(timer.Get(), 0_s);
  wpi::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 500_ms);

  // Verify timer reports 0 ms after reset
  timer.Reset();
  EXPECT_EQ(timer.Get(), 0_s);

  // Verify timer continues incrementing
  wpi::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 500_ms);

  // Verify timer doesn't start incrementing after reset if it was stopped
  timer.Stop();
  timer.Reset();
  wpi::sim::StepTiming(500_ms);
  EXPECT_EQ(timer.Get(), 0_ms);
}

TEST_F(TimerTest, HasElapsed) {
  Timer timer;

  // Verify 0 ms has elapsed since timer hasn't started
  EXPECT_TRUE(timer.HasElapsed(0_s));

  // Verify timer doesn't report elapsed time when stopped
  wpi::sim::StepTiming(500_ms);
  EXPECT_FALSE(timer.HasElapsed(400_ms));

  timer.Start();

  // Verify timer reports >= 400 ms has elapsed after multiple calls
  wpi::sim::StepTiming(500_ms);
  EXPECT_TRUE(timer.HasElapsed(400_ms));
  EXPECT_TRUE(timer.HasElapsed(400_ms));
}

TEST_F(TimerTest, AdvanceIfElapsed) {
  Timer timer;

  // Verify 0 ms has elapsed since timer hasn't started
  EXPECT_TRUE(timer.AdvanceIfElapsed(0_s));

  // Verify timer doesn't report elapsed time when stopped
  wpi::sim::StepTiming(500_ms);
  EXPECT_FALSE(timer.AdvanceIfElapsed(400_ms));

  timer.Start();

  // Verify timer reports >= 400 ms has elapsed for only first call
  wpi::sim::StepTiming(500_ms);
  EXPECT_TRUE(timer.AdvanceIfElapsed(400_ms));
  EXPECT_FALSE(timer.AdvanceIfElapsed(400_ms));

  // Verify timer reports >= 400 ms has elapsed for two calls
  wpi::sim::StepTiming(1_s);
  EXPECT_TRUE(timer.AdvanceIfElapsed(400_ms));
  EXPECT_TRUE(timer.AdvanceIfElapsed(400_ms));
  EXPECT_FALSE(timer.AdvanceIfElapsed(400_ms));
}

TEST_F(TimerTest, GetFPGATimestamp) {
  auto start = wpi::Timer::GetFPGATimestamp();
  wpi::sim::StepTiming(500_ms);
  auto end = wpi::Timer::GetFPGATimestamp();
  EXPECT_EQ(start + 500_ms, end);
}

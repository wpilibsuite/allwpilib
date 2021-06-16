// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Watchdog.h"  // NOLINT(build/include_order)

#include <stdint.h>

#include "frc/simulation/SimHooks.h"
#include "gtest/gtest.h"

using namespace frc;

namespace {
class WatchdogTest : public ::testing::Test {
 protected:
  void SetUp() override { frc::sim::PauseTiming(); }

  void TearDown() override { frc::sim::ResumeTiming(); }
};

}  // namespace

TEST_F(WatchdogTest, EnableDisable) {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4_s, [&] { watchdogCounter++; });

  // Run 1
  watchdog.Enable();
  frc::sim::StepTiming(0.2_s);
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";

  // Run 2
  watchdogCounter = 0;
  watchdog.Enable();
  frc::sim::StepTiming(0.4_s);
  watchdog.Disable();

  EXPECT_EQ(1u, watchdogCounter)
      << "Watchdog either didn't trigger or triggered more than once";

  // Run 3
  watchdogCounter = 0;
  watchdog.Enable();
  frc::sim::StepTiming(1_s);
  watchdog.Disable();

  EXPECT_EQ(1u, watchdogCounter)
      << "Watchdog either didn't trigger or triggered more than once";
}

TEST_F(WatchdogTest, Reset) {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4_s, [&] { watchdogCounter++; });

  watchdog.Enable();
  frc::sim::StepTiming(0.2_s);
  watchdog.Reset();
  frc::sim::StepTiming(0.2_s);
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";
}

TEST_F(WatchdogTest, SetTimeout) {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(1_s, [&] { watchdogCounter++; });

  watchdog.Enable();
  frc::sim::StepTiming(0.2_s);
  watchdog.SetTimeout(0.2_s);

  EXPECT_EQ(0.2_s, watchdog.GetTimeout());
  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";

  frc::sim::StepTiming(0.3_s);
  watchdog.Disable();

  EXPECT_EQ(1u, watchdogCounter)
      << "Watchdog either didn't trigger or triggered more than once";
}

TEST_F(WatchdogTest, IsExpired) {
  Watchdog watchdog(0.2_s, [] {});
  EXPECT_FALSE(watchdog.IsExpired());
  watchdog.Enable();

  EXPECT_FALSE(watchdog.IsExpired());
  frc::sim::StepTiming(0.3_s);
  EXPECT_TRUE(watchdog.IsExpired());

  watchdog.Disable();
  EXPECT_TRUE(watchdog.IsExpired());

  watchdog.Reset();
  EXPECT_FALSE(watchdog.IsExpired());
}

TEST_F(WatchdogTest, Epochs) {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4_s, [&] { watchdogCounter++; });

  // Run 1
  watchdog.Enable();
  watchdog.AddEpoch("Epoch 1");
  frc::sim::StepTiming(0.1_s);
  watchdog.AddEpoch("Epoch 2");
  frc::sim::StepTiming(0.1_s);
  watchdog.AddEpoch("Epoch 3");
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";

  // Run 2
  watchdog.Enable();
  watchdog.AddEpoch("Epoch 1");
  frc::sim::StepTiming(0.2_s);
  watchdog.Reset();
  frc::sim::StepTiming(0.2_s);
  watchdog.AddEpoch("Epoch 2");
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";
}

TEST_F(WatchdogTest, MultiWatchdog) {
  uint32_t watchdogCounter1 = 0;
  uint32_t watchdogCounter2 = 0;

  Watchdog watchdog1(0.2_s, [&] { watchdogCounter1++; });
  Watchdog watchdog2(0.6_s, [&] { watchdogCounter2++; });

  watchdog2.Enable();
  frc::sim::StepTiming(0.25_s);
  EXPECT_EQ(0u, watchdogCounter1) << "Watchdog triggered early";
  EXPECT_EQ(0u, watchdogCounter2) << "Watchdog triggered early";

  // Sleep enough such that only the watchdog enabled later times out first
  watchdog1.Enable();
  frc::sim::StepTiming(0.25_s);
  watchdog1.Disable();
  watchdog2.Disable();

  EXPECT_EQ(1u, watchdogCounter1)
      << "Watchdog either didn't trigger or triggered more than once";
  EXPECT_EQ(0u, watchdogCounter2) << "Watchdog triggered early";
}

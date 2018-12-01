/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Watchdog.h"  // NOLINT(build/include_order)

#include <stdint.h>

#include <thread>

#include <wpi/raw_ostream.h>

#include "gtest/gtest.h"

using namespace frc;

TEST(WatchdogTest, EnableDisable) {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4, [&] { watchdogCounter++; });

  wpi::outs() << "Run 1\n";
  watchdog.Enable();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";

  wpi::outs() << "Run 2\n";
  watchdogCounter = 0;
  watchdog.Enable();
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  watchdog.Disable();

  EXPECT_EQ(1u, watchdogCounter)
      << "Watchdog either didn't trigger or triggered more than once";

  wpi::outs() << "Run 3\n";
  watchdogCounter = 0;
  watchdog.Enable();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  watchdog.Disable();

  EXPECT_EQ(1u, watchdogCounter)
      << "Watchdog either didn't trigger or triggered more than once";
}

TEST(WatchdogTest, Reset) {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4, [&] { watchdogCounter++; });

  watchdog.Enable();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  watchdog.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";
}

TEST(WatchdogTest, SetTimeout) {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(1.0, [&] { watchdogCounter++; });

  watchdog.Enable();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  watchdog.SetTimeout(0.2);

  EXPECT_EQ(0.2, watchdog.GetTimeout());
  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";

  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  watchdog.Disable();

  EXPECT_EQ(1u, watchdogCounter)
      << "Watchdog either didn't trigger or triggered more than once";
}

TEST(WatchdogTest, IsExpired) {
  Watchdog watchdog(0.2, [] {});
  watchdog.Enable();

  EXPECT_FALSE(watchdog.IsExpired());
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  EXPECT_TRUE(watchdog.IsExpired());
}

TEST(WatchdogTest, Epochs) {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4, [&] { watchdogCounter++; });

  wpi::outs() << "Run 1\n";
  watchdog.Enable();
  watchdog.AddEpoch("Epoch 1");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  watchdog.AddEpoch("Epoch 2");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  watchdog.AddEpoch("Epoch 3");
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";

  wpi::outs() << "Run 2\n";
  watchdog.Enable();
  watchdog.AddEpoch("Epoch 1");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  watchdog.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  watchdog.AddEpoch("Epoch 2");
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";
}

TEST(WatchdogTest, MultiWatchdog) {
  uint32_t watchdogCounter1 = 0;
  uint32_t watchdogCounter2 = 0;

  Watchdog watchdog1(0.2, [&] { watchdogCounter1++; });
  Watchdog watchdog2(0.6, [&] { watchdogCounter2++; });

  watchdog2.Enable();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_EQ(0u, watchdogCounter1) << "Watchdog triggered early";
  EXPECT_EQ(0u, watchdogCounter2) << "Watchdog triggered early";

  // Sleep enough such that only the watchdog enabled later times out first
  watchdog1.Enable();
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  watchdog1.Disable();
  watchdog2.Disable();

  EXPECT_EQ(1u, watchdogCounter1)
      << "Watchdog either didn't trigger or triggered more than once";
  EXPECT_EQ(0u, watchdogCounter2) << "Watchdog triggered early";
}

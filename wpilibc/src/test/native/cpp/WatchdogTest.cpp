/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Watchdog.h"  // NOLINT(build/include_order)

#include <stdint.h>

#include <hal/cpp/fpga_clock.h>
#include <mockdata/MockHooks.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>

#include "gtest/gtest.h"

using namespace frc;

namespace frc {
class WatchdogTest {
 public:
  // Mock time in microseconds
  uint64_t mockTime = 0;

  /**
   * Advance mock time by given time delta.
   *
   * @param ms Time delta in milliseconds by which to advance mock time.
   */
  void AdvanceMockTimeBy(uint64_t ms) {
    mockTime += ms * 1e3;
    Watchdog::Notify();
  }
};
}  // namespace frc

static WatchdogTest fixture;

uint64_t GetMockTime() { return fixture.mockTime; }

TEST(WatchdogTest, EnableDisable) {
  wpi::SetNowImpl(GetMockTime);
  HALSIM_RestartTiming();

  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4_s, [&] { watchdogCounter++; });

  wpi::outs() << "Run 1\n";
  watchdog.Enable();
  fixture.AdvanceMockTimeBy(200);
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";

  wpi::outs() << "Run 2\n";
  watchdogCounter = 0;
  watchdog.Enable();
  fixture.AdvanceMockTimeBy(600);
  watchdog.Disable();

  EXPECT_EQ(1u, watchdogCounter)
      << "Watchdog either didn't trigger or triggered more than once";

  wpi::outs() << "Run 3\n";
  watchdogCounter = 0;
  watchdog.Enable();
  fixture.AdvanceMockTimeBy(1000);
  watchdog.Disable();

  EXPECT_EQ(1u, watchdogCounter)
      << "Watchdog either didn't trigger or triggered more than once";
}

TEST(WatchdogTest, Reset) {
  wpi::SetNowImpl(GetMockTime);
  HALSIM_RestartTiming();

  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4_s, [&] { watchdogCounter++; });

  watchdog.Enable();
  fixture.AdvanceMockTimeBy(200);
  watchdog.Reset();
  fixture.AdvanceMockTimeBy(200);
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";
}

TEST(WatchdogTest, SetTimeout) {
  wpi::SetNowImpl(GetMockTime);
  HALSIM_RestartTiming();

  uint32_t watchdogCounter = 0;

  Watchdog watchdog(1.0_s, [&] { watchdogCounter++; });

  watchdog.Enable();
  fixture.AdvanceMockTimeBy(200);
  watchdog.SetTimeout(0.2_s);

  EXPECT_EQ(0.2, watchdog.GetTimeout());
  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";

  fixture.AdvanceMockTimeBy(300);
  watchdog.Disable();

  EXPECT_EQ(1u, watchdogCounter)
      << "Watchdog either didn't trigger or triggered more than once";
}

TEST(WatchdogTest, IsExpired) {
  wpi::SetNowImpl(GetMockTime);
  HALSIM_RestartTiming();

  Watchdog watchdog(0.2_s, [] {});
  EXPECT_FALSE(watchdog.IsExpired());
  watchdog.Enable();

  EXPECT_FALSE(watchdog.IsExpired());
  fixture.AdvanceMockTimeBy(300);
  EXPECT_TRUE(watchdog.IsExpired());

  watchdog.Disable();
  EXPECT_TRUE(watchdog.IsExpired());

  watchdog.Reset();
  EXPECT_FALSE(watchdog.IsExpired());
}

TEST(WatchdogTest, Epochs) {
  wpi::SetNowImpl(GetMockTime);
  HALSIM_RestartTiming();

  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4_s, [&] { watchdogCounter++; });

  wpi::outs() << "Run 1\n";
  watchdog.Enable();
  watchdog.AddEpoch("Epoch 1");
  fixture.AdvanceMockTimeBy(100);
  watchdog.AddEpoch("Epoch 2");
  fixture.AdvanceMockTimeBy(100);
  watchdog.AddEpoch("Epoch 3");
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";

  wpi::outs() << "Run 2\n";
  watchdog.Enable();
  watchdog.AddEpoch("Epoch 1");
  fixture.AdvanceMockTimeBy(200);
  watchdog.Reset();
  fixture.AdvanceMockTimeBy(200);
  watchdog.AddEpoch("Epoch 2");
  watchdog.Disable();

  EXPECT_EQ(0u, watchdogCounter) << "Watchdog triggered early";
}

TEST(WatchdogTest, MultiWatchdog) {
  wpi::SetNowImpl(GetMockTime);
  HALSIM_RestartTiming();

  uint32_t watchdogCounter1 = 0;
  uint32_t watchdogCounter2 = 0;

  Watchdog watchdog1(0.2_s, [&] { watchdogCounter1++; });
  Watchdog watchdog2(0.6_s, [&] { watchdogCounter2++; });

  watchdog2.Enable();
  fixture.AdvanceMockTimeBy(200);
  EXPECT_EQ(0u, watchdogCounter1) << "Watchdog triggered early";
  EXPECT_EQ(0u, watchdogCounter2) << "Watchdog triggered early";

  // Sleep enough such that only the watchdog enabled later times out first
  watchdog1.Enable();
  fixture.AdvanceMockTimeBy(300);
  watchdog1.Disable();
  watchdog2.Disable();

  EXPECT_EQ(1u, watchdogCounter1)
      << "Watchdog either didn't trigger or triggered more than once";
  EXPECT_EQ(0u, watchdogCounter2) << "Watchdog triggered early";
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Watchdog.hpp"

#include <stdint.h>

#include <algorithm>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/hal/HAL.h"
#include "wpi/hal/simulation/MockHooks.h"
#include "wpi/simulation/SimHooks.hpp"

using namespace wpi;

namespace {
class WatchdogTest {
 public:
  WatchdogTest() { wpi::sim::PauseTiming(); }

  ~WatchdogTest() { wpi::sim::ResumeTiming(); }
};

// Steps the paused clock to an odd nanosecond count of at least 2^53. Seconds
// in a double can't represent it, so it catches rounding from converting
// timestamps through seconds.
void StepToLargeClock() {
  int64_t now = HAL_GetMonotonicTime();
  int64_t target = std::max<int64_t>(now, int64_t{1} << 53) | 1;
  HALSIM_StepTiming(target - now);
}

}  // namespace

TEST_CASE_METHOD(WatchdogTest, "WatchdogTest EnableDisable", "[wpilibc]") {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4_s, [&] { watchdogCounter++; });

  // Run 1
  watchdog.Enable();
  wpi::sim::StepTiming(0.2_s);
  watchdog.Disable();

  UNSCOPED_INFO("Watchdog triggered early");
  CHECK(0u == watchdogCounter);

  // Run 2
  watchdogCounter = 0;
  watchdog.Enable();
  wpi::sim::StepTiming(0.4_s);
  watchdog.Disable();

  UNSCOPED_INFO("Watchdog either didn't trigger or triggered more than once");
  CHECK(1u == watchdogCounter);

  // Run 3
  watchdogCounter = 0;
  watchdog.Enable();
  wpi::sim::StepTiming(1_s);
  watchdog.Disable();

  UNSCOPED_INFO("Watchdog either didn't trigger or triggered more than once");
  CHECK(1u == watchdogCounter);
}

TEST_CASE_METHOD(WatchdogTest, "WatchdogTest LargeClock", "[wpilibc]") {
  StepToLargeClock();

  uint32_t watchdogCounter = 0;
  Watchdog watchdog(0.4_s, [&] { watchdogCounter++; });

  watchdog.Enable();
  HALSIM_StepTiming(399'999'999);
  UNSCOPED_INFO("Watchdog triggered early");
  CHECK(0u == watchdogCounter);
  HALSIM_StepTiming(1);
  UNSCOPED_INFO("Watchdog didn't trigger at the timeout");
  CHECK(1u == watchdogCounter);

  watchdogCounter = 0;
  watchdog.SetTimeout(0.4_s);
  HALSIM_StepTiming(200'000'000);
  CHECK_THAT(watchdog.GetTime().value(),
             Catch::Matchers::WithinAbs(0.2, 1e-10));
  HALSIM_StepTiming(199'999'999);
  UNSCOPED_INFO("Watchdog triggered early after SetTimeout");
  CHECK(0u == watchdogCounter);
  HALSIM_StepTiming(1);
  UNSCOPED_INFO("Watchdog didn't trigger at the timeout after SetTimeout");
  CHECK(1u == watchdogCounter);

  watchdog.Disable();
}

TEST_CASE_METHOD(WatchdogTest, "WatchdogTest Reset", "[wpilibc]") {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4_s, [&] { watchdogCounter++; });

  watchdog.Enable();
  wpi::sim::StepTiming(0.2_s);
  watchdog.Reset();
  wpi::sim::StepTiming(0.2_s);
  watchdog.Disable();

  UNSCOPED_INFO("Watchdog triggered early");
  CHECK(0u == watchdogCounter);
}

TEST_CASE_METHOD(WatchdogTest, "WatchdogTest SetTimeout", "[wpilibc]") {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(1_s, [&] { watchdogCounter++; });

  watchdog.Enable();
  wpi::sim::StepTiming(0.2_s);
  watchdog.SetTimeout(0.2_s);

  CHECK(0.2_s == watchdog.GetTimeout());
  UNSCOPED_INFO("Watchdog triggered early");
  CHECK(0u == watchdogCounter);

  wpi::sim::StepTiming(0.3_s);
  watchdog.Disable();

  UNSCOPED_INFO("Watchdog either didn't trigger or triggered more than once");
  CHECK(1u == watchdogCounter);
}

TEST_CASE_METHOD(WatchdogTest, "WatchdogTest IsExpired", "[wpilibc]") {
  Watchdog watchdog(0.2_s, [] {});
  CHECK_FALSE(watchdog.IsExpired());
  watchdog.Enable();

  CHECK_FALSE(watchdog.IsExpired());
  wpi::sim::StepTiming(0.3_s);
  CHECK(watchdog.IsExpired());

  watchdog.Disable();
  CHECK(watchdog.IsExpired());

  watchdog.Reset();
  CHECK_FALSE(watchdog.IsExpired());
}

TEST_CASE_METHOD(WatchdogTest, "WatchdogTest Epochs", "[wpilibc]") {
  uint32_t watchdogCounter = 0;

  Watchdog watchdog(0.4_s, [&] { watchdogCounter++; });

  // Run 1
  watchdog.Enable();
  watchdog.AddEpoch("Epoch 1");
  wpi::sim::StepTiming(0.1_s);
  watchdog.AddEpoch("Epoch 2");
  wpi::sim::StepTiming(0.1_s);
  watchdog.AddEpoch("Epoch 3");
  watchdog.Disable();

  UNSCOPED_INFO("Watchdog triggered early");
  CHECK(0u == watchdogCounter);

  // Run 2
  watchdog.Enable();
  watchdog.AddEpoch("Epoch 1");
  wpi::sim::StepTiming(0.2_s);
  watchdog.Reset();
  wpi::sim::StepTiming(0.2_s);
  watchdog.AddEpoch("Epoch 2");
  watchdog.Disable();

  UNSCOPED_INFO("Watchdog triggered early");
  CHECK(0u == watchdogCounter);
}

TEST_CASE_METHOD(WatchdogTest, "WatchdogTest MultiWatchdog", "[wpilibc]") {
  uint32_t watchdogCounter1 = 0;
  uint32_t watchdogCounter2 = 0;

  Watchdog watchdog1(0.2_s, [&] { watchdogCounter1++; });
  Watchdog watchdog2(0.6_s, [&] { watchdogCounter2++; });

  watchdog2.Enable();
  wpi::sim::StepTiming(0.25_s);
  UNSCOPED_INFO("Watchdog triggered early");
  CHECK(0u == watchdogCounter1);
  UNSCOPED_INFO("Watchdog triggered early");
  CHECK(0u == watchdogCounter2);

  // Sleep enough such that only the watchdog enabled later times out first
  watchdog1.Enable();
  wpi::sim::StepTiming(0.25_s);
  watchdog1.Disable();
  watchdog2.Disable();

  UNSCOPED_INFO("Watchdog either didn't trigger or triggered more than once");
  CHECK(1u == watchdogCounter1);
  UNSCOPED_INFO("Watchdog triggered early");
  CHECK(0u == watchdogCounter2);
}

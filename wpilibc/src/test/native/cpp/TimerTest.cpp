// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Timer.hpp"

#include <stdint.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/simulation/SimHooks.hpp"
#include "wpi/system/RobotController.hpp"

using namespace wpi;

namespace {
int64_t mockTime = 0;

class ScopedMockTimeSource {
 public:
  ScopedMockTimeSource() {
    RobotController::SetTimeSource([] { return mockTime; });
  }

  ~ScopedMockTimeSource() {
    RobotController::SetTimeSource(
        [] { return RobotController::GetMonotonicTime(); });
  }
};

class TimerTest {
 public:
  TimerTest() {
    mockTime = 0;
    wpi::sim::PauseTiming();
    wpi::sim::RestartTiming();
  }

  ~TimerTest() { wpi::sim::ResumeTiming(); }
};

}  // namespace

TEST_CASE_METHOD(TimerTest, "TimerTest StartStop", "[wpilibc]") {
  Timer timer;

  // Verify timer is initialized as stopped
  CHECK(timer.Get() == 0_s);
  CHECK_FALSE(timer.IsRunning());
  wpi::sim::StepTiming(500_ms);
  CHECK(timer.Get() == 0_s);
  CHECK_FALSE(timer.IsRunning());

  // Verify timer increments after it's started
  timer.Start();
  wpi::sim::StepTiming(500_ms);
  CHECK(timer.Get() == 500_ms);
  CHECK(timer.IsRunning());

  // Verify timer stops incrementing after it's stopped
  timer.Stop();
  wpi::sim::StepTiming(500_ms);
  CHECK(timer.Get() == 500_ms);
  CHECK_FALSE(timer.IsRunning());
}

TEST_CASE_METHOD(TimerTest, "TimerTest CreateStarted", "[wpilibc]") {
  Timer timer = Timer::CreateStarted();
  CHECK(timer.IsRunning());
}

TEST_CASE_METHOD(TimerTest, "TimerTest Reset", "[wpilibc]") {
  Timer timer;
  timer.Start();

  // Advance timer to 500 ms
  CHECK(timer.Get() == 0_s);
  wpi::sim::StepTiming(500_ms);
  CHECK(timer.Get() == 500_ms);

  // Verify timer reports 0 ms after reset
  timer.Reset();
  CHECK(timer.Get() == 0_s);

  // Verify timer continues incrementing
  wpi::sim::StepTiming(500_ms);
  CHECK(timer.Get() == 500_ms);

  // Verify timer doesn't start incrementing after reset if it was stopped
  timer.Stop();
  timer.Reset();
  wpi::sim::StepTiming(500_ms);
  CHECK(timer.Get() == 0_ms);
}

TEST_CASE_METHOD(TimerTest, "TimerTest ResetWithLargeTimestamp", "[wpilibc]") {
  ScopedMockTimeSource timeSource;
  mockTime = 1'000'000'000LL;

  Timer timer;
  timer.Start();

  mockTime += 500'000'000;
  CHECK(timer.Get() == 500_ms);

  timer.Reset();
  CHECK(timer.Get() == 0_s);

  mockTime += 500'000'000;
  CHECK(timer.Get() == 500_ms);
}

TEST_CASE_METHOD(TimerTest, "TimerTest HasElapsed", "[wpilibc]") {
  Timer timer;

  // Verify 0 ms has elapsed since timer hasn't started
  CHECK(timer.HasElapsed(0_s));

  // Verify timer doesn't report elapsed time when stopped
  wpi::sim::StepTiming(500_ms);
  CHECK_FALSE(timer.HasElapsed(400_ms));

  timer.Start();

  // Verify timer reports >= 400 ms has elapsed after multiple calls
  wpi::sim::StepTiming(500_ms);
  CHECK(timer.HasElapsed(400_ms));
  CHECK(timer.HasElapsed(400_ms));
}

TEST_CASE_METHOD(TimerTest, "TimerTest AdvanceIfElapsed", "[wpilibc]") {
  Timer timer;

  // Verify 0 ms has elapsed since timer hasn't started
  CHECK(timer.AdvanceIfElapsed(0_s));

  // Verify timer doesn't report elapsed time when stopped
  wpi::sim::StepTiming(500_ms);
  CHECK_FALSE(timer.AdvanceIfElapsed(400_ms));

  timer.Start();

  // Verify timer reports >= 400 ms has elapsed for only first call
  wpi::sim::StepTiming(500_ms);
  CHECK(timer.AdvanceIfElapsed(400_ms));
  CHECK_FALSE(timer.AdvanceIfElapsed(400_ms));

  // Verify timer reports >= 400 ms has elapsed for two calls
  wpi::sim::StepTiming(1_s);
  CHECK(timer.AdvanceIfElapsed(400_ms));
  CHECK(timer.AdvanceIfElapsed(400_ms));
  CHECK_FALSE(timer.AdvanceIfElapsed(400_ms));
}

TEST_CASE_METHOD(TimerTest,
                 "TimerTest AdvanceIfElapsedPreservesFractionalPeriod",
                 "[wpilibc]") {
  ScopedMockTimeSource timeSource;
  mockTime = 0;

  Timer timer;
  timer.Start();

  auto period = wpi::units::seconds<>{1.0 / 60.0};

  for (int64_t i = 1; i <= 60; ++i) {
    mockTime = (i * 1'000'000'000LL + 59) / 60 + 100;

    CHECK(timer.AdvanceIfElapsed(period));
    CHECK_FALSE(timer.AdvanceIfElapsed(period));
  }

  CHECK_THAT(timer.Get().value(), Catch::Matchers::WithinAbs(100e-9, 1e-12));
}

TEST_CASE_METHOD(TimerTest,
                 "TimerTest AdvanceIfElapsedProgressesWithSubNanosecondPeriod",
                 "[wpilibc]") {
  ScopedMockTimeSource timeSource;
  mockTime = 0;

  Timer timer;
  timer.Start();

  mockTime = 1;
  auto period = wpi::units::nanoseconds<>{0.1};

  for (int i = 0; i < 10; ++i) {
    CHECK(timer.AdvanceIfElapsed(period));
  }

  CHECK_FALSE(timer.AdvanceIfElapsed(period));
  CHECK_THAT(timer.Get().value(), Catch::Matchers::WithinAbs(0.0, 1e-12));
}

TEST_CASE_METHOD(TimerTest, "TimerTest GetMonotonicTimestamp", "[wpilibc]") {
  auto start = wpi::Timer::GetMonotonicTimestamp();
  wpi::sim::StepTiming(500_ms);
  auto end = wpi::Timer::GetMonotonicTimestamp();
  CHECK(start + 500_ms == end);
}

TEST_CASE_METHOD(TimerTest, "TimerTest RestartTimingPreservesPausedClock",
                 "[wpilibc]") {
  CHECK(wpi::sim::IsTimingPaused());

  wpi::sim::StepTiming(500_ms);
  auto beforeRestart = wpi::Timer::GetMonotonicTimestamp();

  wpi::sim::RestartTiming();

  CHECK(wpi::sim::IsTimingPaused());
  CHECK(beforeRestart == wpi::Timer::GetMonotonicTimestamp());

  wpi::sim::StepTiming(500_ms);
  CHECK(beforeRestart + 500_ms == wpi::Timer::GetMonotonicTimestamp());
}

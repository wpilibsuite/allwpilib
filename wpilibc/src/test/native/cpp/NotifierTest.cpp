// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Notifier.hpp"

#include <atomic>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/simulation/SimHooks.hpp"

using namespace wpi;

namespace {

class NotifierTest {
 public:
  NotifierTest() {
    sim::PauseTiming();
    sim::RestartTiming();
  }

  ~NotifierTest() { sim::ResumeTiming(); }
};

}  // namespace

TEST_CASE_METHOD(NotifierTest, "NotifierTest StartPeriodicAndStop",
                 "[wpilibc]") {
  std::atomic<uint32_t> counter{0};

  Notifier notifier{[&] { ++counter; }};
  notifier.StartPeriodic(1_s);

  sim::StepTiming(10.5_s);

  notifier.Stop();
  CHECK((10u) == (counter));

  sim::StepTiming(3_s);

  CHECK((10u) == (counter));
}

TEST_CASE_METHOD(NotifierTest, "NotifierTest StartSingle", "[wpilibc]") {
  std::atomic<uint32_t> counter{0};

  Notifier notifier{[&] { ++counter; }};
  notifier.StartSingle(1_s);

  sim::StepTiming(10.5_s);

  CHECK((1u) == (counter));
}

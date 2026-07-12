// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Notifier.hpp"

#include <atomic>

#include <gtest/gtest.h>

#include "wpi/simulation/SimHooks.hpp"

using namespace wpi;

namespace {

class NotifierTest : public ::testing::Test {
 protected:
  void SetUp() override {
    sim::PauseTiming();
    sim::RestartTiming();
  }

  void TearDown() override { sim::ResumeTiming(); }
};

}  // namespace

TEST_F(NotifierTest, StartPeriodicAndStop) {
  std::atomic<uint32_t> counter{0};

  Notifier notifier{[&] { ++counter; }};
  notifier.StartPeriodic(1_s);

  sim::StepTiming(10.5_s);

  notifier.Stop();
  EXPECT_EQ(10u, counter);

  sim::StepTiming(3_s);

  EXPECT_EQ(10u, counter);
}

TEST_F(NotifierTest, StartSingle) {
  std::atomic<uint32_t> counter{0};

  Notifier notifier{[&] { ++counter; }};
  notifier.StartSingle(1_s);

  sim::StepTiming(10.5_s);

  EXPECT_EQ(1u, counter);
}

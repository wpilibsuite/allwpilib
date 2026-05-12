// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/StepNodeLogic.hpp"

#include <gtest/gtest.h>

#include "wpi/filterdesigner/model/Signal.hpp"

namespace {

using wpi::filterdesigner::Signal;
using wpi::filterdesigner::StepNodeLogic;

TEST(StepNodeLogicTest, DefaultsProduceUnitStepStartingAtZero) {
  StepNodeLogic logic;
  const Signal* sig = logic.Signal();
  ASSERT_NE(sig, nullptr);
  ASSERT_FALSE(sig->values.empty());
  for (double v : sig->values) {
    EXPECT_DOUBLE_EQ(v, 1.0);
  }
  EXPECT_EQ(sig->values.size(), 200u);
  EXPECT_DOUBLE_EQ(sig->sampleRate, 1000.0);
}

TEST(StepNodeLogicTest, StartSampleDelaysStep) {
  StepNodeLogic logic;
  logic.length = 10;
  logic.startSample = 4;
  const Signal* sig = logic.Signal();
  ASSERT_NE(sig, nullptr);
  for (std::size_t i = 0; i < 4; ++i) {
    EXPECT_DOUBLE_EQ(sig->values[i], 0.0);
  }
  for (std::size_t i = 4; i < 10; ++i) {
    EXPECT_DOUBLE_EQ(sig->values[i], 1.0);
  }
}

TEST(StepNodeLogicTest, StartSampleClampedIntoRange) {
  StepNodeLogic logic;
  logic.length = 5;
  logic.startSample = 99;  // out of range; should clamp to length-1
  const Signal* sig = logic.Signal();
  ASSERT_NE(sig, nullptr);
  // Only the last sample is 1.
  EXPECT_DOUBLE_EQ(sig->values[0], 0.0);
  EXPECT_DOUBLE_EQ(sig->values[3], 0.0);
  EXPECT_DOUBLE_EQ(sig->values[4], 1.0);
}

TEST(StepNodeLogicTest, RepeatedCallsReturnSamePointerWhenParamsUnchanged) {
  StepNodeLogic logic;
  const Signal* a = logic.Signal();
  const Signal* b = logic.Signal();
  EXPECT_EQ(a, b);
}

TEST(StepNodeLogicTest, ChangingStartSampleBumpsRevisionAndRebuilds) {
  StepNodeLogic logic;
  logic.length = 10;
  logic.startSample = 0;
  const Signal* a = logic.Signal();
  ASSERT_NE(a, nullptr);
  std::uint64_t rev0 = a->revision;
  logic.startSample = 5;
  const Signal* b = logic.Signal();
  ASSERT_NE(b, nullptr);
  EXPECT_GT(b->revision, rev0);
  EXPECT_DOUBLE_EQ(b->values[4], 0.0);
  EXPECT_DOUBLE_EQ(b->values[5], 1.0);
}

TEST(StepNodeLogicTest, InvalidParamsReturnNull) {
  StepNodeLogic logic;
  logic.sampleRate = 0.0;
  EXPECT_EQ(logic.Signal(), nullptr);
  logic.sampleRate = 1000.0;
  logic.length = 1;
  EXPECT_EQ(logic.Signal(), nullptr);
}

}  // namespace

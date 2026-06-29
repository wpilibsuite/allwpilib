// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/ImpulseNodeLogic.hpp"

#include <gtest/gtest.h>

#include "wpi/filterdesigner/model/Signal.hpp"

namespace {

using wpi::filterdesigner::ImpulseNodeLogic;
using wpi::filterdesigner::Signal;

TEST(ImpulseNodeLogicTest, DefaultsProduceUnitImpulse) {
  ImpulseNodeLogic logic;
  const Signal* sig = logic.Signal();
  ASSERT_NE(sig, nullptr);
  ASSERT_FALSE(sig->values.empty());
  EXPECT_DOUBLE_EQ(sig->values.front(), 1.0);
  for (std::size_t i = 1; i < sig->values.size(); ++i) {
    EXPECT_DOUBLE_EQ(sig->values[i], 0.0) << "tail must be zero at i=" << i;
  }
  EXPECT_DOUBLE_EQ(sig->sampleRate, 1000.0);
  EXPECT_EQ(sig->values.size(), 200u);
  EXPECT_TRUE(sig->uniform);
}

TEST(ImpulseNodeLogicTest, TimestampsAreUniformlySpaced) {
  ImpulseNodeLogic logic;
  logic.sampleRate = 500.0;
  logic.length = 8;
  const Signal* sig = logic.Signal();
  ASSERT_NE(sig, nullptr);
  ASSERT_EQ(sig->timestamps.size(), 8u);
  for (std::size_t i = 0; i < sig->timestamps.size(); ++i) {
    EXPECT_DOUBLE_EQ(sig->timestamps[i], static_cast<double>(i) / 500.0);
  }
}

TEST(ImpulseNodeLogicTest, RepeatedCallsReturnSamePointerWhenParamsUnchanged) {
  ImpulseNodeLogic logic;
  const Signal* a = logic.Signal();
  const Signal* b = logic.Signal();
  EXPECT_EQ(a, b);
}

TEST(ImpulseNodeLogicTest, ChangingLengthBumpsRevisionAndRebuilds) {
  ImpulseNodeLogic logic;
  const Signal* a = logic.Signal();
  ASSERT_NE(a, nullptr);
  std::uint64_t rev0 = a->revision;
  logic.length = 50;
  const Signal* b = logic.Signal();
  ASSERT_NE(b, nullptr);
  EXPECT_EQ(b->values.size(), 50u);
  EXPECT_GT(b->revision, rev0);
}

TEST(ImpulseNodeLogicTest, ChangingSampleRateRebuildsSignal) {
  ImpulseNodeLogic logic;
  logic.sampleRate = 1000.0;
  logic.length = 4;
  const Signal* a = logic.Signal();
  ASSERT_NE(a, nullptr);
  std::uint64_t rev0 = a->revision;
  logic.sampleRate = 4000.0;
  const Signal* b = logic.Signal();
  ASSERT_NE(b, nullptr);
  EXPECT_GT(b->revision, rev0);
  EXPECT_DOUBLE_EQ(b->sampleRate, 4000.0);
  EXPECT_DOUBLE_EQ(b->timestamps[1], 1.0 / 4000.0);
}

TEST(ImpulseNodeLogicTest, InvalidParamsReturnNull) {
  ImpulseNodeLogic logic;
  logic.sampleRate = 0.0;
  EXPECT_EQ(logic.Signal(), nullptr);
  logic.sampleRate = 1000.0;
  logic.length = 1;  // below kMinLength
  EXPECT_EQ(logic.Signal(), nullptr);
}

}  // namespace

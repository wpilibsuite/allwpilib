// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/StepNodeLogic.hpp"

#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"

namespace {

using wpi::filterdesigner::Signal;
using wpi::filterdesigner::StepNodeLogic;

TEST_CASE("StepNodeLogicTest DefaultsProduceUnitStepStartingAtZero",
          "[filterdesigner]") {
  StepNodeLogic logic;
  const Signal* sig = logic.Signal();
  REQUIRE(sig != nullptr);
  REQUIRE_FALSE(sig->values.empty());
  for (double v : sig->values) {
    CHECK_DOUBLE_EQ(v, 1.0);
  }
  CHECK(sig->values.size() == 200u);
  CHECK_DOUBLE_EQ(sig->sampleRate, 1000.0);
  UNSCOPED_INFO("a step is a transient: the Frequency Plot skips the window");
  CHECK(sig->transient);
}

TEST_CASE("StepNodeLogicTest StartSampleDelaysStep", "[filterdesigner]") {
  StepNodeLogic logic;
  logic.length = 10;
  logic.startSample = 4;
  const Signal* sig = logic.Signal();
  REQUIRE(sig != nullptr);
  for (std::size_t i = 0; i < 4; ++i) {
    CHECK_DOUBLE_EQ(sig->values[i], 0.0);
  }
  for (std::size_t i = 4; i < 10; ++i) {
    CHECK_DOUBLE_EQ(sig->values[i], 1.0);
  }
}

TEST_CASE("StepNodeLogicTest StartSampleClampedIntoRange", "[filterdesigner]") {
  StepNodeLogic logic;
  logic.length = 5;
  logic.startSample = 99;  // out of range; should clamp to length-1
  const Signal* sig = logic.Signal();
  REQUIRE(sig != nullptr);
  // Only the last sample is 1.
  CHECK_DOUBLE_EQ(sig->values[0], 0.0);
  CHECK_DOUBLE_EQ(sig->values[3], 0.0);
  CHECK_DOUBLE_EQ(sig->values[4], 1.0);
}

TEST_CASE("StepNodeLogicTest RepeatedCallsReturnSamePointerWhenParamsUnchanged",
          "[filterdesigner]") {
  StepNodeLogic logic;
  const Signal* a = logic.Signal();
  const Signal* b = logic.Signal();
  CHECK(a == b);
}

TEST_CASE("StepNodeLogicTest ChangingStartSampleBumpsRevisionAndRebuilds",
          "[filterdesigner]") {
  StepNodeLogic logic;
  logic.length = 10;
  logic.startSample = 0;
  const Signal* a = logic.Signal();
  REQUIRE(a != nullptr);
  std::uint64_t rev0 = a->revision;
  logic.startSample = 5;
  const Signal* b = logic.Signal();
  REQUIRE(b != nullptr);
  CHECK(b->revision > rev0);
  CHECK_DOUBLE_EQ(b->values[4], 0.0);
  CHECK_DOUBLE_EQ(b->values[5], 1.0);
}

TEST_CASE("StepNodeLogicTest InvalidParamsReturnNull", "[filterdesigner]") {
  StepNodeLogic logic;
  logic.sampleRate = 0.0;
  CHECK(logic.Signal() == nullptr);
  logic.sampleRate = 1000.0;
  logic.length = 1;
  CHECK(logic.Signal() == nullptr);
}

TEST_CASE("StepNodeLogicTest NonFiniteSampleRateReturnsNull",
          "[filterdesigner]") {
  // "1e999" in the sample-rate field parses to infinity, which a plain
  // sampleRate <= 0 test lets through.
  StepNodeLogic logic;
  logic.sampleRate = std::numeric_limits<double>::infinity();
  CHECK(logic.Signal() == nullptr);
  logic.sampleRate = std::numeric_limits<double>::quiet_NaN();
  CHECK(logic.Signal() == nullptr);
  logic.sampleRate = 1000.0;
  CHECK(logic.Signal() != nullptr);
}

}  // namespace

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/ImpulseNodeLogic.hpp"

#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"

namespace {

using wpi::filterdesigner::ImpulseNodeLogic;
using wpi::filterdesigner::Signal;

TEST_CASE("ImpulseNodeLogicTest DefaultsProduceUnitImpulse",
          "[filterdesigner]") {
  ImpulseNodeLogic logic;
  const Signal* sig = logic.Signal();
  REQUIRE(sig != nullptr);
  REQUIRE_FALSE(sig->values.empty());
  CHECK_DOUBLE_EQ(sig->values.front(), 1.0);
  for (std::size_t i = 1; i < sig->values.size(); ++i) {
    UNSCOPED_INFO("tail must be zero at i=" << i);
    CHECK_DOUBLE_EQ(sig->values[i], 0.0);
  }
  CHECK_DOUBLE_EQ(sig->sampleRate, 1000.0);
  CHECK(sig->values.size() == 200u);
  CHECK(sig->quality.onGrid);
  UNSCOPED_INFO(
      "an impulse is a transient: the Frequency Plot skips the window");
  CHECK(sig->transient);
}

TEST_CASE("ImpulseNodeLogicTest TimestampsAreUniformlySpaced",
          "[filterdesigner]") {
  ImpulseNodeLogic logic;
  logic.sampleRate = 500.0;
  logic.length = 8;
  const Signal* sig = logic.Signal();
  REQUIRE(sig != nullptr);
  REQUIRE(sig->timestamps.size() == 8u);
  for (std::size_t i = 0; i < sig->timestamps.size(); ++i) {
    CHECK_DOUBLE_EQ(sig->timestamps[i], static_cast<double>(i) / 500.0);
  }
}

TEST_CASE(
    "ImpulseNodeLogicTest RepeatedCallsReturnSamePointerWhenParamsUnchanged",
    "[filterdesigner]") {
  ImpulseNodeLogic logic;
  const Signal* a = logic.Signal();
  const Signal* b = logic.Signal();
  CHECK(a == b);
}

TEST_CASE("ImpulseNodeLogicTest ChangingLengthBumpsRevisionAndRebuilds",
          "[filterdesigner]") {
  ImpulseNodeLogic logic;
  const Signal* a = logic.Signal();
  REQUIRE(a != nullptr);
  std::uint64_t rev0 = a->revision;
  logic.length = 50;
  const Signal* b = logic.Signal();
  REQUIRE(b != nullptr);
  CHECK(b->values.size() == 50u);
  CHECK(b->revision > rev0);
}

TEST_CASE("ImpulseNodeLogicTest ChangingSampleRateRebuildsSignal",
          "[filterdesigner]") {
  ImpulseNodeLogic logic;
  logic.sampleRate = 1000.0;
  logic.length = 4;
  const Signal* a = logic.Signal();
  REQUIRE(a != nullptr);
  std::uint64_t rev0 = a->revision;
  logic.sampleRate = 4000.0;
  const Signal* b = logic.Signal();
  REQUIRE(b != nullptr);
  CHECK(b->revision > rev0);
  CHECK_DOUBLE_EQ(b->sampleRate, 4000.0);
  CHECK_DOUBLE_EQ(b->timestamps[1], 1.0 / 4000.0);
}

TEST_CASE("ImpulseNodeLogicTest InvalidParamsReturnNull", "[filterdesigner]") {
  ImpulseNodeLogic logic;
  logic.sampleRate = 0.0;
  CHECK(logic.Signal() == nullptr);
  logic.sampleRate = 1000.0;
  logic.length = 1;  // below kMinLength
  CHECK(logic.Signal() == nullptr);
}

TEST_CASE("ImpulseNodeLogicTest NonFiniteSampleRateReturnsNull",
          "[filterdesigner]") {
  // "1e999" in the sample-rate field parses to infinity, which a plain
  // sampleRate <= 0 test lets through.
  ImpulseNodeLogic logic;
  logic.sampleRate = std::numeric_limits<double>::infinity();
  CHECK(logic.Signal() == nullptr);
  logic.sampleRate = std::numeric_limits<double>::quiet_NaN();
  CHECK(logic.Signal() == nullptr);
  logic.sampleRate = 1000.0;
  CHECK(logic.Signal() != nullptr);
}

}  // namespace

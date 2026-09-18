// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/NT4SourceNodeLogic.hpp"

#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/filterdesigner/io/NT4Source.hpp"
#include "wpi/nt/ntcore_c.h"

namespace {

using wpi::filterdesigner::NT4Source;
using wpi::filterdesigner::NT4SourceNodeLogic;

TEST_CASE("NT4SourceNodeLogicTest DefaultsMatchNetworkTablesConventions",
          "[filterdesigner]") {
  NT4SourceNodeLogic logic;
  CHECK(logic.serverMode == NT4SourceNodeLogic::ServerMode::Host);
  CHECK(logic.host == "127.0.0.1");
  CHECK(logic.team == 0);
  CHECK(logic.port == static_cast<int>(NT_DEFAULT_PORT));
  CHECK(logic.TopicName().empty());
  CHECK_DOUBLE_EQ(logic.BufferSeconds(), 30.0);
  CHECK_FALSE(logic.Frozen());
}

TEST_CASE("NT4SourceNodeLogicTest SignalReturnsNullWhenBufferEmpty",
          "[filterdesigner]") {
  NT4SourceNodeLogic logic;
  logic.Update();  // no drain set + no samples
  CHECK(logic.Signal() == nullptr);
}

TEST_CASE("NT4SourceNodeLogicTest SignalReturnsStablePointerOnceSamplesArrive",
          "[filterdesigner]") {
  NT4SourceNodeLogic logic;
  bool drained = false;
  logic.SetDrain([&drained]() {
    if (drained) {
      return std::vector<NT4Source::Sample>{};
    }
    drained = true;
    return std::vector<NT4Source::Sample>{{100'000, 1.0}, {200'000, 2.0}};
  });
  logic.Update();
  const auto* first = logic.Signal();
  REQUIRE(first != nullptr);
  logic.Update();
  CHECK(logic.Signal() == first);
}

TEST_CASE("NT4SourceNodeLogicTest SetTopicNamePropagatesToSignalName",
          "[filterdesigner]") {
  NT4SourceNodeLogic logic;
  logic.SetTopicName("/SmartDashboard/shooter/rpm");
  CHECK(logic.TopicName() == "/SmartDashboard/shooter/rpm");
  CHECK(logic.Source().GetSignal()->name == "/SmartDashboard/shooter/rpm");
}

TEST_CASE("NT4SourceNodeLogicTest ClearDropsBufferedSamplesButKeepsTopic",
          "[filterdesigner]") {
  NT4SourceNodeLogic logic;
  logic.SetTopicName("/foo");
  bool drained = false;
  logic.SetDrain([&drained]() {
    if (drained) {
      return std::vector<NT4Source::Sample>{};
    }
    drained = true;
    return std::vector<NT4Source::Sample>{{100'000, 1.0}, {200'000, 2.0}};
  });
  logic.Update();
  REQUIRE(logic.SampleCount() == 2u);
  logic.Clear();
  CHECK(logic.SampleCount() == 0u);
  // Topic identity follows the subscription, not the buffer.
  CHECK(logic.TopicName() == "/foo");
  CHECK(logic.Source().GetSignal()->name == "/foo");
}

TEST_CASE("NT4SourceNodeLogicTest BufferSecondsValidatedToPositive",
          "[filterdesigner]") {
  NT4SourceNodeLogic logic;
  logic.SetBufferSeconds(20.0);
  logic.SetBufferSeconds(0.0);
  CHECK_DOUBLE_EQ(logic.BufferSeconds(), 20.0);
  logic.SetBufferSeconds(-3.0);
  CHECK_DOUBLE_EQ(logic.BufferSeconds(), 20.0);
}

TEST_CASE("NT4SourceNodeLogicTest FrozenIgnoresDrainedSamples",
          "[filterdesigner]") {
  NT4SourceNodeLogic logic;
  bool drained = false;
  logic.SetDrain([&drained]() {
    if (drained) {
      return std::vector<NT4Source::Sample>{};
    }
    drained = true;
    return std::vector<NT4Source::Sample>{{100'000, 1.0}, {200'000, 2.0}};
  });
  logic.SetFrozen(true);
  logic.Update();
  CHECK(logic.SampleCount() == 0u);
  CHECK(logic.Signal() == nullptr);
}

TEST_CASE("NT4SourceNodeLogicTest SanitizeTeamClampsNegativeToZero",
          "[filterdesigner]") {
  CHECK(NT4SourceNodeLogic::SanitizeTeam(0) == 0);
  CHECK(NT4SourceNodeLogic::SanitizeTeam(254) == 254);
  CHECK(NT4SourceNodeLogic::SanitizeTeam(-1) == 0);
}

TEST_CASE("NT4SourceNodeLogicTest SanitizePortFallsBackToDefaultOnNonPositive",
          "[filterdesigner]") {
  CHECK(NT4SourceNodeLogic::SanitizePort(5810) ==
        static_cast<int>(NT_DEFAULT_PORT));
  CHECK(NT4SourceNodeLogic::SanitizePort(0) ==
        static_cast<int>(NT_DEFAULT_PORT));
  CHECK(NT4SourceNodeLogic::SanitizePort(-7) ==
        static_cast<int>(NT_DEFAULT_PORT));
  CHECK(NT4SourceNodeLogic::SanitizePort(9001) == 9001);
}

TEST_CASE("NT4SourceNodeLogicTest UpdateWithNoDrainIsNoOp",
          "[filterdesigner]") {
  NT4SourceNodeLogic logic;
  logic.Update();
  logic.Update();
  CHECK(logic.SampleCount() == 0u);
  CHECK(logic.Signal() == nullptr);
}

TEST_CASE("NT4SourceNodeLogicTest SanitizeTeamClampsToNtcoreAddressRange",
          "[filterdesigner]") {
  // ntcore only forms 10.TE.AM.2 for teams 0 through 25599.
  CHECK(NT4SourceNodeLogic::SanitizeTeam(25599) == 25599);
  CHECK(NT4SourceNodeLogic::SanitizeTeam(25600) == 25599);
  CHECK(NT4SourceNodeLogic::SanitizeTeam(100000) == 25599);
}

}  // namespace

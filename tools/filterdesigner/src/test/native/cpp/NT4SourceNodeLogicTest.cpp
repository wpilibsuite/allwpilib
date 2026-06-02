// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/NT4SourceNodeLogic.hpp"

#include <vector>

#include <gtest/gtest.h>

#include "wpi/filterdesigner/io/NT4Source.hpp"
#include "wpi/nt/ntcore_c.h"

namespace {

using wpi::filterdesigner::NT4Source;
using wpi::filterdesigner::NT4SourceNodeLogic;

TEST(NT4SourceNodeLogicTest, DefaultsMatchNetworkTablesConventions) {
  NT4SourceNodeLogic logic;
  EXPECT_EQ(logic.serverMode, NT4SourceNodeLogic::ServerMode::Host);
  EXPECT_EQ(logic.host, "127.0.0.1");
  EXPECT_EQ(logic.team, 0);
  EXPECT_EQ(logic.port, static_cast<int>(NT_DEFAULT_PORT));
  EXPECT_TRUE(logic.TopicName().empty());
  EXPECT_DOUBLE_EQ(logic.BufferSeconds(), 30.0);
  EXPECT_FALSE(logic.Frozen());
}

TEST(NT4SourceNodeLogicTest, SignalReturnsNullWhenBufferEmpty) {
  NT4SourceNodeLogic logic;
  logic.Update();  // no drain set + no samples
  EXPECT_EQ(logic.Signal(), nullptr);
}

TEST(NT4SourceNodeLogicTest, SignalReturnsStablePointerOnceSamplesArrive) {
  NT4SourceNodeLogic logic;
  bool drained = false;
  logic.SetDrain([&drained]() {
    if (drained) {
      return std::vector<NT4Source::Sample>{};
    }
    drained = true;
    return std::vector<NT4Source::Sample>{{100, 1.0}, {200, 2.0}};
  });
  logic.Update();
  const auto* first = logic.Signal();
  ASSERT_NE(first, nullptr);
  logic.Update();
  EXPECT_EQ(logic.Signal(), first);
}

TEST(NT4SourceNodeLogicTest, SetTopicNamePropagatesToSignalName) {
  NT4SourceNodeLogic logic;
  logic.SetTopicName("/SmartDashboard/shooter/rpm");
  EXPECT_EQ(logic.TopicName(), "/SmartDashboard/shooter/rpm");
  EXPECT_EQ(logic.Source().GetSignal()->name, "/SmartDashboard/shooter/rpm");
}

TEST(NT4SourceNodeLogicTest, ClearDropsBufferedSamplesButKeepsTopic) {
  NT4SourceNodeLogic logic;
  logic.SetTopicName("/foo");
  bool drained = false;
  logic.SetDrain([&drained]() {
    if (drained) {
      return std::vector<NT4Source::Sample>{};
    }
    drained = true;
    return std::vector<NT4Source::Sample>{{100, 1.0}, {200, 2.0}};
  });
  logic.Update();
  ASSERT_EQ(logic.SampleCount(), 2u);
  logic.Clear();
  EXPECT_EQ(logic.SampleCount(), 0u);
  // Topic identity follows the subscription, not the buffer.
  EXPECT_EQ(logic.TopicName(), "/foo");
  EXPECT_EQ(logic.Source().GetSignal()->name, "/foo");
}

TEST(NT4SourceNodeLogicTest, BufferSecondsValidatedToPositive) {
  NT4SourceNodeLogic logic;
  logic.SetBufferSeconds(20.0);
  logic.SetBufferSeconds(0.0);
  EXPECT_DOUBLE_EQ(logic.BufferSeconds(), 20.0);
  logic.SetBufferSeconds(-3.0);
  EXPECT_DOUBLE_EQ(logic.BufferSeconds(), 20.0);
}

TEST(NT4SourceNodeLogicTest, FrozenIgnoresDrainedSamples) {
  NT4SourceNodeLogic logic;
  bool drained = false;
  logic.SetDrain([&drained]() {
    if (drained) {
      return std::vector<NT4Source::Sample>{};
    }
    drained = true;
    return std::vector<NT4Source::Sample>{{100, 1.0}, {200, 2.0}};
  });
  logic.SetFrozen(true);
  logic.Update();
  EXPECT_EQ(logic.SampleCount(), 0u);
  EXPECT_EQ(logic.Signal(), nullptr);
}

TEST(NT4SourceNodeLogicTest, SanitizeTeamClampsNegativeToZero) {
  EXPECT_EQ(NT4SourceNodeLogic::SanitizeTeam(0), 0);
  EXPECT_EQ(NT4SourceNodeLogic::SanitizeTeam(254), 254);
  EXPECT_EQ(NT4SourceNodeLogic::SanitizeTeam(-1), 0);
}

TEST(NT4SourceNodeLogicTest, SanitizePortFallsBackToDefaultOnNonPositive) {
  EXPECT_EQ(NT4SourceNodeLogic::SanitizePort(5810),
            static_cast<int>(NT_DEFAULT_PORT));
  EXPECT_EQ(NT4SourceNodeLogic::SanitizePort(0),
            static_cast<int>(NT_DEFAULT_PORT));
  EXPECT_EQ(NT4SourceNodeLogic::SanitizePort(-7),
            static_cast<int>(NT_DEFAULT_PORT));
  EXPECT_EQ(NT4SourceNodeLogic::SanitizePort(9001), 9001);
}

TEST(NT4SourceNodeLogicTest, UpdateWithNoDrainIsNoOp) {
  NT4SourceNodeLogic logic;
  logic.Update();
  logic.Update();
  EXPECT_EQ(logic.SampleCount(), 0u);
  EXPECT_EQ(logic.Signal(), nullptr);
}

}  // namespace

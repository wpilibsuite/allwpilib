// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>

#include <ImNodeFlow.h>
#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/graph/Serialize.hpp"
#include "wpi/filterdesigner/nodes/NT4SourceNode.hpp"
#include "wpi/filterdesigner/nodes/NT4SourceNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/TimePlotNode.hpp"
#include "wpi/nt/ntcore_c.h"

namespace {

using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::NT4SourceNode;
using wpi::filterdesigner::NT4SourceNodeLogic;
using wpi::filterdesigner::SerializeGraph;
using wpi::filterdesigner::TimePlotNode;

void RegisterAll(NodeRegistry& reg) {
  NT4SourceNode::Register(reg);
  TimePlotNode::Register(reg);
}

TEST_CASE("NT4SourceNodeSerializeTest ServerSettingsRoundTrip",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto node = graph.AddNode<NT4SourceNode>(ImVec2{15.0f, 25.0f});
  node->Logic().serverMode = NT4SourceNodeLogic::ServerMode::Team;
  node->Logic().host = "10.12.34.2";
  node->Logic().team = 1234;
  node->Logic().port = 5810;
  int id = node->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().serverMode == NT4SourceNodeLogic::ServerMode::Team);
  CHECK(loaded->Logic().host == "10.12.34.2");
  CHECK(loaded->Logic().team == 1234);
  CHECK(loaded->Logic().port == 5810);
}

TEST_CASE("NT4SourceNodeSerializeTest TopicAndBufferSettingsRoundTrip",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto node = graph.AddNode<NT4SourceNode>(ImVec2{0.0f, 0.0f});
  node->Logic().SetTopicName("/SmartDashboard/shooter/rpm");
  node->Logic().SetBufferSeconds(12.5);
  node->Logic().SetFrozen(true);
  int id = node->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().TopicName() == "/SmartDashboard/shooter/rpm");
  CHECK_DOUBLE_EQ(loaded->Logic().BufferSeconds(), 12.5);
  CHECK(loaded->Logic().Frozen());
  // Signal name follows the topic.
  CHECK(loaded->Logic().Source().GetSignal()->name ==
        "/SmartDashboard/shooter/rpm");
}

TEST_CASE("NT4SourceNodeSerializeTest NT4SourceToTimePlotLinkRoundTrips",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto source = graph.AddNode<NT4SourceNode>(ImVec2{0.0f, 0.0f});
  auto plot = graph.AddNode<TimePlotNode>(ImVec2{300.0f, 0.0f});
  source->Logic().SetTopicName("/foo");
  plot->inPin("in0")->createLink(source->outPin("out"));
  int srcId = source->GraphId();
  int plotId = plot->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto links = restored.Links();
  REQUIRE(links.size() == 1u);
  CHECK(links[0].srcId == srcId);
  CHECK(links[0].dstId == plotId);
  CHECK(links[0].srcPin == "out");
  CHECK(links[0].dstPin == "in0");
}

TEST_CASE("NT4SourceNodeSerializeTest SanitizesNegativeTeamAndPortOnLoad",
          "[filterdesigner]") {
  // Hand-rolled JSON with out-of-range values — guards against a stale .fdsgn
  // (or hand-edited file) wiring the live UI up with nonsense.
  NodeRegistry reg;
  RegisterAll(reg);
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "NT4Source", "pos": [0, 0],
       "serverMode": 0, "host": "10.0.0.2",
       "team": -42, "port": 0, "topic": "", "bufferSeconds": 30.0,
       "frozen": false}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().team == 0);
  CHECK(loaded->Logic().port == static_cast<int>(NT_DEFAULT_PORT));
}

TEST_CASE("NT4SourceNodeSerializeTest MissingFieldsUseDefaults",
          "[filterdesigner]") {
  // Forward-compat: a graph written by a future build that drops a field, or
  // a hand-rolled file with the minimum keys present.
  NodeRegistry reg;
  RegisterAll(reg);
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 7, "type": "NT4Source", "pos": [10, 20]}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(7));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().serverMode == NT4SourceNodeLogic::ServerMode::Host);
  CHECK(loaded->Logic().host == "127.0.0.1");
  CHECK(loaded->Logic().port == static_cast<int>(NT_DEFAULT_PORT));
  CHECK(loaded->Logic().TopicName().empty());
  CHECK_DOUBLE_EQ(loaded->Logic().BufferSeconds(), 30.0);
  CHECK_FALSE(loaded->Logic().Frozen());
}

TEST_CASE("NT4SourceNodeSerializeTest SignalNullUntilDrainProducesSamples",
          "[filterdesigner]") {
  // A freshly-loaded node neither reconnects nor invents placeholder data, so
  // the out pin stays null and downstream sinks skip rendering.
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto node = graph.AddNode<NT4SourceNode>(ImVec2{0.0f, 0.0f});
  node->Logic().SetTopicName("/foo");
  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded =
      dynamic_cast<NT4SourceNode*>(restored.FindNodeById(node->GraphId()));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().Signal() == nullptr);
}

TEST_CASE(
    "NT4SourceNodeSerializeTest ConstructionDoesNotAllocateNtcoreInstance",
    "[filterdesigner]") {
  // The wrapped NetworkTableInstance is lazy: creating it in the ctor would
  // leak an ntcore handle for every node a graph load constructs.
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto node = graph.AddNode<NT4SourceNode>(ImVec2{0.0f, 0.0f});
  UNSCOPED_INFO("NT instance must stay null until the user clicks Connect");
  CHECK_FALSE(node->IsInstanceCreated());
}

TEST_CASE("NT4SourceNodeSerializeTest SanitizePortCapsAboveMaxOnLoad",
          "[filterdesigner]") {
  // The upper bound: a hand-edited port of 99999 shouldn't reach ntcore.
  NodeRegistry reg;
  RegisterAll(reg);
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "NT4Source", "pos": [0, 0],
       "serverMode": 1, "host": "127.0.0.1", "team": 0, "port": 99999,
       "topic": "", "bufferSeconds": 5.0, "frozen": false}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().port <= 65535);
  CHECK(loaded->Logic().port >= 1);
}

TEST_CASE("NT4SourceNodeSerializeTest OutOfRangeIntFieldsKeepDefaults",
          "[filterdesigner]") {
  // A negative team is sanitized after the cast; a value that is not an int
  // at all never reaches the cast, so the defaults stand instead.
  NodeRegistry reg;
  RegisterAll(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "NT4Source", "pos": [0, 0],
       "serverMode": 1e100, "team": 1e100, "port": 2.5}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().serverMode == NT4SourceNodeLogic::ServerMode::Host);
  CHECK(loaded->Logic().team == 0);
  CHECK(loaded->Logic().port == 5810);
}

}  // namespace

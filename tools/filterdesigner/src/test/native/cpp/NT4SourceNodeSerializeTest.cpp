// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>

#include <ImNodeFlow.h>
#include <gtest/gtest.h>

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

TEST(NT4SourceNodeSerializeTest, ServerSettingsRoundTrip) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(id));
  ASSERT_NE(loaded, nullptr);
  EXPECT_EQ(loaded->Logic().serverMode, NT4SourceNodeLogic::ServerMode::Team);
  EXPECT_EQ(loaded->Logic().host, "10.12.34.2");
  EXPECT_EQ(loaded->Logic().team, 1234);
  EXPECT_EQ(loaded->Logic().port, 5810);
}

TEST(NT4SourceNodeSerializeTest, TopicAndBufferSettingsRoundTrip) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(id));
  ASSERT_NE(loaded, nullptr);
  EXPECT_EQ(loaded->Logic().TopicName(), "/SmartDashboard/shooter/rpm");
  EXPECT_DOUBLE_EQ(loaded->Logic().BufferSeconds(), 12.5);
  EXPECT_TRUE(loaded->Logic().Frozen());
  // Signal name follows the topic.
  EXPECT_EQ(loaded->Logic().Source().GetSignal()->name,
            "/SmartDashboard/shooter/rpm");
}

TEST(NT4SourceNodeSerializeTest, NT4SourceToTimePlotLinkRoundTrips) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto links = restored.Links();
  ASSERT_EQ(links.size(), 1u);
  EXPECT_EQ(links[0].srcId, srcId);
  EXPECT_EQ(links[0].dstId, plotId);
  EXPECT_EQ(links[0].srcPin, "out");
  EXPECT_EQ(links[0].dstPin, "in0");
}

TEST(NT4SourceNodeSerializeTest, SanitizesNegativeTeamAndPortOnLoad) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(1));
  ASSERT_NE(loaded, nullptr);
  EXPECT_EQ(loaded->Logic().team, 0);
  EXPECT_EQ(loaded->Logic().port, static_cast<int>(NT_DEFAULT_PORT));
}

TEST(NT4SourceNodeSerializeTest, MissingFieldsUseDefaults) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(7));
  ASSERT_NE(loaded, nullptr);
  EXPECT_EQ(loaded->Logic().serverMode, NT4SourceNodeLogic::ServerMode::Host);
  EXPECT_EQ(loaded->Logic().host, "127.0.0.1");
  EXPECT_EQ(loaded->Logic().port, static_cast<int>(NT_DEFAULT_PORT));
  EXPECT_TRUE(loaded->Logic().TopicName().empty());
  EXPECT_DOUBLE_EQ(loaded->Logic().BufferSeconds(), 30.0);
  EXPECT_FALSE(loaded->Logic().Frozen());
}

TEST(NT4SourceNodeSerializeTest, SignalNullUntilDrainProducesSamples) {
  // The OutPin behaviour returns Logic::Signal() which is null while the
  // buffer is empty — downstream sinks rely on that null check to skip
  // rendering. Make sure a freshly-loaded node still honors that contract
  // (no auto-reconnect, no fake placeholder data).
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto node = graph.AddNode<NT4SourceNode>(ImVec2{0.0f, 0.0f});
  node->Logic().SetTopicName("/foo");
  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded =
      dynamic_cast<NT4SourceNode*>(restored.FindNodeById(node->GraphId()));
  ASSERT_NE(loaded, nullptr);
  EXPECT_EQ(loaded->Logic().Signal(), nullptr);
}

TEST(NT4SourceNodeSerializeTest, ConstructionDoesNotAllocateNtcoreInstance) {
  // The wrapped NetworkTableInstance must be lazy. If a future refactor
  // reverts to eagerly Create()ing in the ctor, every test in this suite
  // (and every node ever instantiated on graph load) would silently leak
  // an ntcore handle. Lock the contract via the public accessor instead
  // of leaning on inspection.
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto node = graph.AddNode<NT4SourceNode>(ImVec2{0.0f, 0.0f});
  EXPECT_FALSE(node->IsInstanceCreated())
      << "NT instance must stay null until the user clicks Connect";
}

TEST(NT4SourceNodeSerializeTest, SanitizePortCapsAboveMaxOnLoad) {
  // SanitizePort now caps at 65535 — a hand-edited file with port: 99999
  // shouldn't pass nonsense to ntcore. Pairs with the existing
  // SanitizesNegativeTeamAndPortOnLoad case which only exercises the lower
  // bound.
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<NT4SourceNode*>(restored.FindNodeById(1));
  ASSERT_NE(loaded, nullptr);
  EXPECT_LE(loaded->Logic().port, 65535);
  EXPECT_GE(loaded->Logic().port, 1);
}

}  // namespace

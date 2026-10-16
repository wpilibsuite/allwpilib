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
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#include "wpi/filterdesigner/nodes/ImpulseNode.hpp"
#include "wpi/filterdesigner/nodes/ImpulseNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/TimePlotNode.hpp"

namespace {

using wpi::filterdesigner::BiquadStageNode;
using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::ImpulseNode;
using wpi::filterdesigner::ImpulseNodeLogic;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;
using wpi::filterdesigner::TimePlotNode;

TEST_CASE("ImpulseNodeSerializeTest ParamsRoundTrip", "[filterdesigner]") {
  NodeRegistry reg;
  ImpulseNode::Register(reg);

  Graph graph;
  auto src = graph.AddNode<ImpulseNode>(ImVec2{10.0f, 20.0f});
  src->Logic().sampleRate = 2500.0;
  src->Logic().length = 64;
  int id = src->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<ImpulseNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK_DOUBLE_EQ(loaded->Logic().sampleRate, 2500.0);
  CHECK(loaded->Logic().length == 64);
}

TEST_CASE("ImpulseNodeSerializeTest LengthClampedOnLoad", "[filterdesigner]") {
  NodeRegistry reg;
  ImpulseNode::Register(reg);

  // Length out of range — loader should clamp into the supported range.
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "Impulse", "pos": [0, 0],
       "sampleRate": 1000.0, "length": 99999999}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<ImpulseNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().length == ImpulseNodeLogic::kMaxLength);
}

TEST_CASE("ImpulseNodeSerializeTest ToBiquadStageToTimePlotChainRoundTrips",
          "[filterdesigner]") {
  NodeRegistry reg;
  ImpulseNode::Register(reg);
  BiquadStageNode::Register(reg);
  TimePlotNode::Register(reg);

  // The canonical impulse-response graph: source → stage → plot.
  Graph graph;
  auto src = graph.AddNode<ImpulseNode>(ImVec2{0.0f, 0.0f});
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  auto plot = graph.AddNode<TimePlotNode>(ImVec2{400.0f, 0.0f});
  stage->inPin("in")->createLink(src->outPin("out"));
  plot->inPin("in0")->createLink(stage->outPin("signal"));

  int srcId = src->GraphId();
  int stageId = stage->GraphId();
  int plotId = plot->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(restored.Links().size() == 2u);

  bool foundSrcToStage = false;
  bool foundStageToPlot = false;
  for (const auto& link : restored.Links()) {
    if (link.srcId == srcId && link.dstId == stageId && link.srcPin == "out" &&
        link.dstPin == "in") {
      foundSrcToStage = true;
    } else if (link.srcId == stageId && link.dstId == plotId &&
               link.srcPin == "signal" && link.dstPin == "in0") {
      foundStageToPlot = true;
    }
  }
  CHECK(foundSrcToStage);
  CHECK(foundStageToPlot);
}

TEST_CASE("ImpulseNodeSerializeTest OutOfRangeLengthKeepsDefault",
          "[filterdesigner]") {
  // Unlike an in-range but too-large length, which is clamped, a value that
  // is not an int at all is refused and the default stands.
  NodeRegistry reg;
  ImpulseNode::Register(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "Impulse", "pos": [0, 0], "length": 1e100}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<ImpulseNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().length == 200);
}

}  // namespace

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
#include "wpi/filterdesigner/model/PoleZero.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#include "wpi/filterdesigner/nodes/PoleZeroPlotNode.hpp"
#include "wpi/filterdesigner/nodes/PoleZeroPlotNodeLogic.hpp"

namespace {

using wpi::filterdesigner::BiquadStageNode;
using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::PoleZeroPlotNode;
using wpi::filterdesigner::SerializeGraph;

TEST_CASE("PoleZeroPlotNodeSerializeTest ParamsRoundTrip", "[filterdesigner]") {
  NodeRegistry reg;
  PoleZeroPlotNode::Register(reg);

  Graph graph;
  auto plot = graph.AddNode<PoleZeroPlotNode>(ImVec2{70.0f, 80.0f});
  plot->Logic().showLegend = false;
  plot->Logic().plotWidth = 500.0f;
  plot->Logic().plotHeight = 500.0f;
  int id = plot->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<PoleZeroPlotNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK_FALSE(loaded->Logic().showLegend);
  CHECK_FLOAT_EQ(loaded->Logic().plotWidth, 500.0f);
  CHECK_FLOAT_EQ(loaded->Logic().plotHeight, 500.0f);
}

TEST_CASE(
    "PoleZeroPlotNodeSerializeTest BiquadStageToPoleZeroPlotLinkRoundTrips",
    "[filterdesigner]") {
  NodeRegistry reg;
  BiquadStageNode::Register(reg);
  PoleZeroPlotNode::Register(reg);

  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{50.0f, 100.0f});
  auto pz = graph.AddNode<PoleZeroPlotNode>(ImVec2{400.0f, 100.0f});
  pz->inPin("in0")->createLink(stage->outPin("filter"));
  int stageId = stage->GraphId();
  int pzId = pz->GraphId();

  std::string json = SerializeGraph(graph);
  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto links = restored.Links();
  REQUIRE(links.size() == 1u);
  CHECK(links[0].srcId == stageId);
  CHECK(links[0].dstId == pzId);
  CHECK(links[0].srcPin == "filter");
  CHECK(links[0].dstPin == "in0");
}

TEST_CASE("PoleZeroPlotNodeSerializeTest ConsumesUpstreamFilterMath",
          "[filterdesigner]") {
  // The math draw() would run, on the wire content it would run it against —
  // ComputePolesZeros over the upstream cascade, no ImGui needed.
  NodeRegistry reg;
  BiquadStageNode::Register(reg);
  PoleZeroPlotNode::Register(reg);

  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto pz = graph.AddNode<PoleZeroPlotNode>(ImVec2{200.0f, 0.0f});
  stage->Logic().sampleRate = 1000.0;
  pz->inPin("in0")->createLink(stage->outPin("filter"));

  const auto* upstream = stage->CombinedFilter();
  REQUIRE(upstream != nullptr);
  auto roots = wpi::filterdesigner::ComputePolesZeros(upstream->sections);
  UNSCOPED_INFO("default BiquadStage should produce at least one pole or zero");
  CHECK_FALSE((roots.poles.empty() && roots.zeros.empty()));
}

}  // namespace

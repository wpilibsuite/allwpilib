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

TEST(PoleZeroPlotNodeSerializeTest, ParamsRoundTrip) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<PoleZeroPlotNode*>(restored.FindNodeById(id));
  ASSERT_NE(loaded, nullptr);
  EXPECT_FALSE(loaded->Logic().showLegend);
  EXPECT_FLOAT_EQ(loaded->Logic().plotWidth, 500.0f);
  EXPECT_FLOAT_EQ(loaded->Logic().plotHeight, 500.0f);
}

TEST(PoleZeroPlotNodeSerializeTest, BiquadStageToPoleZeroPlotLinkRoundTrips) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto links = restored.Links();
  ASSERT_EQ(links.size(), 1u);
  EXPECT_EQ(links[0].srcId, stageId);
  EXPECT_EQ(links[0].dstId, pzId);
  EXPECT_EQ(links[0].srcPin, "filter");
  EXPECT_EQ(links[0].dstPin, "in0");
}

TEST(PoleZeroPlotNodeSerializeTest, ConsumesUpstreamFilterMath) {
  // Verifies the math the PoleZeroPlot's draw() runs against its upstream
  // filter, without needing an ImGui context. ComputePolesZeros must yield
  // at least one root for a reasonable BiquadStage cascade — pre-fix, no
  // test pulled the Filter wire through the plot's data path.
  NodeRegistry reg;
  BiquadStageNode::Register(reg);
  PoleZeroPlotNode::Register(reg);

  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto pz = graph.AddNode<PoleZeroPlotNode>(ImVec2{200.0f, 0.0f});
  stage->Logic().sampleRate = 1000.0;
  pz->inPin("in0")->createLink(stage->outPin("filter"));

  const auto* upstream = stage->CombinedFilter();
  ASSERT_NE(upstream, nullptr);
  auto roots = wpi::filterdesigner::ComputePolesZeros(upstream->sections);
  EXPECT_FALSE(roots.poles.empty() && roots.zeros.empty())
      << "default BiquadStage should produce at least one pole or zero";
}

}  // namespace

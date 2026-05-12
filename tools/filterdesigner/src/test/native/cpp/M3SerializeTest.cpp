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
#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/BodePlotNode.hpp"
#include "wpi/filterdesigner/nodes/BodePlotNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/WpiLogSourceNode.hpp"

namespace {

using wpi::filterdesigner::BiquadStageNode;
using wpi::filterdesigner::BodePlotNode;
using wpi::filterdesigner::BodePlotNodeLogic;
using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::Family;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;
using wpi::filterdesigner::StageKind;
using wpi::filterdesigner::WpiLogSourceNode;

void RegisterAll(NodeRegistry& reg) {
  WpiLogSourceNode::Register(reg);
  BiquadStageNode::Register(reg);
  BodePlotNode::Register(reg);
}

TEST(M3SerializeTest, BiquadStageParamsRoundTrip) {
  NodeRegistry reg;
  RegisterAll(reg);

  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{50.0f, 100.0f});
  stage->Logic().sampleRate = 2500.0;
  stage->Logic().stage.kind = StageKind::HighPass;
  stage->Logic().stage.family = Family::Chebyshev1;
  stage->Logic().stage.order = 6;
  stage->Logic().stage.f1 = 250.0;
  stage->Logic().stage.passbandRippleDb = 1.5;

  int stageId = stage->GraphId();
  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded =
      dynamic_cast<BiquadStageNode*>(restored.FindNodeById(stageId));
  ASSERT_NE(loaded, nullptr);
  EXPECT_DOUBLE_EQ(loaded->Logic().sampleRate, 2500.0);
  EXPECT_EQ(loaded->Logic().stage.kind, StageKind::HighPass);
  EXPECT_EQ(loaded->Logic().stage.family, Family::Chebyshev1);
  EXPECT_EQ(loaded->Logic().stage.order, 6);
  EXPECT_DOUBLE_EQ(loaded->Logic().stage.f1, 250.0);
  EXPECT_DOUBLE_EQ(loaded->Logic().stage.passbandRippleDb, 1.5);
}

TEST(M3SerializeTest, BodePlotParamsRoundTrip) {
  NodeRegistry reg;
  RegisterAll(reg);

  Graph graph;
  auto bode = graph.AddNode<BodePlotNode>(ImVec2{400.0f, 100.0f});
  bode->Logic().autoscale = false;
  bode->Logic().showLegend = false;
  bode->Logic().numPoints = 1024;
  bode->Logic().plotWidth = 720.0f;
  bode->Logic().plotHeight = 410.0f;
  int bodeId = bode->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<BodePlotNode*>(restored.FindNodeById(bodeId));
  ASSERT_NE(loaded, nullptr);
  EXPECT_FALSE(loaded->Logic().autoscale);
  EXPECT_FALSE(loaded->Logic().showLegend);
  EXPECT_EQ(loaded->Logic().numPoints, 1024);
  EXPECT_FLOAT_EQ(loaded->Logic().plotWidth, 720.0f);
  EXPECT_FLOAT_EQ(loaded->Logic().plotHeight, 410.0f);
}

TEST(M3SerializeTest, BodePlotNumPointsClampedOnLoad) {
  NodeRegistry reg;
  RegisterAll(reg);

  // numPoints out of range — loader should clamp into [kMinPoints, kMaxPoints].
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "BodePlot", "pos": [0, 0], "numPoints": 999999}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<BodePlotNode*>(restored.FindNodeById(1));
  ASSERT_NE(loaded, nullptr);
  EXPECT_EQ(loaded->Logic().numPoints, BodePlotNodeLogic::kMaxPoints);
}

TEST(M3SerializeTest, BiquadStageToBodePlotLinkRoundTrips) {
  NodeRegistry reg;
  RegisterAll(reg);

  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{50.0f, 100.0f});
  auto bode = graph.AddNode<BodePlotNode>(ImVec2{400.0f, 100.0f});
  bode->inPin("in0")->createLink(stage->outPin("filter"));

  int stageId = stage->GraphId();
  int bodeId = bode->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto links = restored.Links();
  ASSERT_EQ(links.size(), 1u);
  EXPECT_EQ(links[0].srcId, stageId);
  EXPECT_EQ(links[0].dstId, bodeId);
  EXPECT_EQ(links[0].srcPin, "filter");
  EXPECT_EQ(links[0].dstPin, "in0");
}

TEST(M3SerializeTest, BiquadStageSignalPassthroughLinkRoundTrips) {
  NodeRegistry reg;
  RegisterAll(reg);

  // WpiLogSource → BiquadStage(signal) → next BiquadStage(in). Mirrors the
  // canonical chain shape M3 unlocks; verifies pin-name resolution for both
  // BiquadStage's outputs.
  Graph graph;
  auto src = graph.AddNode<WpiLogSourceNode>(ImVec2{0.0f, 0.0f});
  auto a = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  auto b = graph.AddNode<BiquadStageNode>(ImVec2{400.0f, 0.0f});
  a->inPin("in")->createLink(src->outPin("out"));
  b->inPin("in")->createLink(a->outPin("signal"));

  std::string json = SerializeGraph(graph);
  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  ASSERT_EQ(restored.Links().size(), 2u);

  bool foundSrcToA = false;
  bool foundAToB = false;
  for (const auto& link : restored.Links()) {
    if (link.srcPin == "out" && link.dstPin == "in") {
      foundSrcToA = true;
    } else if (link.srcPin == "signal" && link.dstPin == "in") {
      foundAToB = true;
    }
  }
  EXPECT_TRUE(foundSrcToA);
  EXPECT_TRUE(foundAToB);
}

}  // namespace

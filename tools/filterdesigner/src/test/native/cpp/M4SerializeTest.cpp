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
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#include "wpi/filterdesigner/nodes/FrequencyPlotNode.hpp"
#include "wpi/filterdesigner/nodes/FrequencyPlotNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/ImpulseNode.hpp"
#include "wpi/filterdesigner/nodes/ImpulseNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/PoleZeroPlotNode.hpp"
#include "wpi/filterdesigner/nodes/PoleZeroPlotNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/StepNode.hpp"
#include "wpi/filterdesigner/nodes/StepNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/TimePlotNode.hpp"

namespace {

using wpi::filterdesigner::BiquadStageNode;
using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::FrequencyPlotNode;
using wpi::filterdesigner::FrequencyPlotNodeLogic;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::ImpulseNode;
using wpi::filterdesigner::ImpulseNodeLogic;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::PoleZeroPlotNode;
using wpi::filterdesigner::PoleZeroPlotNodeLogic;
using wpi::filterdesigner::SerializeGraph;
using wpi::filterdesigner::StepNode;
using wpi::filterdesigner::TimePlotNode;

void RegisterAll(NodeRegistry& reg) {
  ImpulseNode::Register(reg);
  StepNode::Register(reg);
  BiquadStageNode::Register(reg);
  TimePlotNode::Register(reg);
  FrequencyPlotNode::Register(reg);
  PoleZeroPlotNode::Register(reg);
}

TEST(M4SerializeTest, ImpulseParamsRoundTrip) {
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto src = graph.AddNode<ImpulseNode>(ImVec2{10.0f, 20.0f});
  src->Logic().sampleRate = 2500.0;
  src->Logic().length = 64;
  int id = src->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<ImpulseNode*>(restored.FindNodeById(id));
  ASSERT_NE(loaded, nullptr);
  EXPECT_DOUBLE_EQ(loaded->Logic().sampleRate, 2500.0);
  EXPECT_EQ(loaded->Logic().length, 64);
}

TEST(M4SerializeTest, ImpulseLengthClampedOnLoad) {
  NodeRegistry reg;
  RegisterAll(reg);

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
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<ImpulseNode*>(restored.FindNodeById(1));
  ASSERT_NE(loaded, nullptr);
  EXPECT_EQ(loaded->Logic().length, ImpulseNodeLogic::kMaxLength);
}

TEST(M4SerializeTest, StepParamsRoundTrip) {
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto src = graph.AddNode<StepNode>(ImVec2{30.0f, 40.0f});
  src->Logic().sampleRate = 200.0;
  src->Logic().length = 32;
  src->Logic().startSample = 5;
  int id = src->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<StepNode*>(restored.FindNodeById(id));
  ASSERT_NE(loaded, nullptr);
  EXPECT_DOUBLE_EQ(loaded->Logic().sampleRate, 200.0);
  EXPECT_EQ(loaded->Logic().length, 32);
  EXPECT_EQ(loaded->Logic().startSample, 5);
}

TEST(M4SerializeTest, FrequencyPlotParamsRoundTrip) {
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto plot = graph.AddNode<FrequencyPlotNode>(ImVec2{50.0f, 60.0f});
  plot->Logic().autoscale = false;
  plot->Logic().showLegend = false;
  plot->Logic().logFrequency = true;
  plot->Logic().plotWidth = 480.0f;
  plot->Logic().plotHeight = 280.0f;
  int id = plot->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<FrequencyPlotNode*>(restored.FindNodeById(id));
  ASSERT_NE(loaded, nullptr);
  EXPECT_FALSE(loaded->Logic().autoscale);
  EXPECT_FALSE(loaded->Logic().showLegend);
  EXPECT_TRUE(loaded->Logic().logFrequency);
  EXPECT_FLOAT_EQ(loaded->Logic().plotWidth, 480.0f);
  EXPECT_FLOAT_EQ(loaded->Logic().plotHeight, 280.0f);
}

TEST(M4SerializeTest, FrequencyPlotSizeClampedToMinOnLoad) {
  NodeRegistry reg;
  RegisterAll(reg);
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FrequencyPlot", "pos": [0, 0],
       "plotWidth": 1.0, "plotHeight": 1.0}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<FrequencyPlotNode*>(restored.FindNodeById(1));
  ASSERT_NE(loaded, nullptr);
  EXPECT_FLOAT_EQ(loaded->Logic().plotWidth,
                  FrequencyPlotNodeLogic::kMinPlotWidth);
  EXPECT_FLOAT_EQ(loaded->Logic().plotHeight,
                  FrequencyPlotNodeLogic::kMinPlotHeight);
}

TEST(M4SerializeTest, PoleZeroPlotParamsRoundTrip) {
  NodeRegistry reg;
  RegisterAll(reg);
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

TEST(M4SerializeTest, ImpulseToBiquadStageToTimePlotChainRoundTrips) {
  NodeRegistry reg;
  RegisterAll(reg);

  // Canonical "impulse response" graph — the why-nodes example from the plan.
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
  ASSERT_TRUE(result.ok()) << result.error;
  ASSERT_EQ(restored.Links().size(), 2u);

  bool foundSrcToStage = false;
  bool foundStageToPlot = false;
  for (const auto& link : restored.Links()) {
    if (link.srcId == srcId && link.dstId == stageId &&
        link.srcPin == "out" && link.dstPin == "in") {
      foundSrcToStage = true;
    } else if (link.srcId == stageId && link.dstId == plotId &&
               link.srcPin == "signal" && link.dstPin == "in0") {
      foundStageToPlot = true;
    }
  }
  EXPECT_TRUE(foundSrcToStage);
  EXPECT_TRUE(foundStageToPlot);
}

TEST(M4SerializeTest, BiquadStageToPoleZeroPlotLinkRoundTrips) {
  NodeRegistry reg;
  RegisterAll(reg);
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

}  // namespace

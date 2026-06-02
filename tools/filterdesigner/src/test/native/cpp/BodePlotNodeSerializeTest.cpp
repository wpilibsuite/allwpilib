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
#include "wpi/filterdesigner/nodes/BodePlotNode.hpp"
#include "wpi/filterdesigner/nodes/BodePlotNodeLogic.hpp"

namespace {

using wpi::filterdesigner::BodePlotNode;
using wpi::filterdesigner::BodePlotNodeLogic;
using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;

TEST(BodePlotNodeSerializeTest, ParamsRoundTrip) {
  NodeRegistry reg;
  BodePlotNode::Register(reg);

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

TEST(BodePlotNodeSerializeTest, NumPointsClampedOnLoad) {
  NodeRegistry reg;
  BodePlotNode::Register(reg);

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

}  // namespace

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
#include "wpi/filterdesigner/nodes/StepNode.hpp"
#include "wpi/filterdesigner/nodes/StepNodeLogic.hpp"

namespace {

using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;
using wpi::filterdesigner::StepNode;

TEST(StepNodeSerializeTest, ParamsRoundTrip) {
  NodeRegistry reg;
  StepNode::Register(reg);

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

}  // namespace

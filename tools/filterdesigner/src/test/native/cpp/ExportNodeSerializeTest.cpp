// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>

#include <ImNodeFlow.h>
#include <gtest/gtest.h>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/graph/Serialize.hpp"
#include "wpi/filterdesigner/nodes/ExportNode.hpp"
#include "wpi/filterdesigner/nodes/ExportNodeLogic.hpp"

namespace {

using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::ExportNode;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::Language;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;

TEST(ExportNodeSerializeTest, ParamsRoundTrip) {
  NodeRegistry reg;
  ExportNode::Register(reg);

  Graph graph;
  auto node = graph.AddNode<ExportNode>(ImVec2{30.0f, 40.0f});
  node->Logic().lang = Language::Java;
  node->Logic().className = "ShooterFilter";
  node->Logic().projectRoot = "/path/to/robot";
  int id = node->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<ExportNode*>(restored.FindNodeById(id));
  ASSERT_NE(loaded, nullptr);
  EXPECT_EQ(loaded->Logic().lang, Language::Java);
  EXPECT_EQ(loaded->Logic().className, "ShooterFilter");
  EXPECT_EQ(loaded->Logic().projectRoot, "/path/to/robot");
}

}  // namespace

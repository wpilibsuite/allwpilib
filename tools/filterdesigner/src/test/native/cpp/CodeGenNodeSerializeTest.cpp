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
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#include "wpi/filterdesigner/nodes/CodeGenNode.hpp"
#include "wpi/filterdesigner/nodes/CodeGenNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/ExportNode.hpp"

namespace {

using wpi::filterdesigner::BiquadStageNode;
using wpi::filterdesigner::CodeGenNode;
using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::ExportNode;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::Language;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;

TEST(CodeGenNodeSerializeTest, ParamsRoundTrip) {
  NodeRegistry reg;
  CodeGenNode::Register(reg);

  Graph graph;
  auto node = graph.AddNode<CodeGenNode>(ImVec2{10.0f, 20.0f});
  node->Logic().lang = Language::Python;
  node->Logic().varName = "shooterFilter";
  int id = node->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<CodeGenNode*>(restored.FindNodeById(id));
  ASSERT_NE(loaded, nullptr);
  EXPECT_EQ(loaded->Logic().lang, Language::Python);
  EXPECT_EQ(loaded->Logic().varName, "shooterFilter");
}

TEST(CodeGenNodeSerializeTest, EmptyVarNameDefaultsOnLoad) {
  NodeRegistry reg;
  CodeGenNode::Register(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "CodeGen", "pos": [0, 0],
       "lang": 0, "varName": ""}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<CodeGenNode*>(restored.FindNodeById(1));
  ASSERT_NE(loaded, nullptr);
  EXPECT_EQ(loaded->Logic().varName, "filter");
}

TEST(CodeGenNodeSerializeTest, BiquadStageToCodeGenLinkRoundTrips) {
  NodeRegistry reg;
  BiquadStageNode::Register(reg);
  CodeGenNode::Register(reg);

  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto codegen = graph.AddNode<CodeGenNode>(ImVec2{300.0f, 0.0f});
  codegen->inPin("in")->createLink(stage->outPin("filter"));
  int stageId = stage->GraphId();
  int codegenId = codegen->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  auto links = restored.Links();
  ASSERT_EQ(links.size(), 1u);
  EXPECT_EQ(links[0].srcId, stageId);
  EXPECT_EQ(links[0].dstId, codegenId);
  EXPECT_EQ(links[0].srcPin, "filter");
  EXPECT_EQ(links[0].dstPin, "in");
}

TEST(CodeGenNodeSerializeTest, BiquadStageToMultipleSinksRoundTrips) {
  // Multi-language export off one Filter wire — the canonical "why
  // CodeGen-as-sink" example from the plan.
  NodeRegistry reg;
  BiquadStageNode::Register(reg);
  CodeGenNode::Register(reg);
  ExportNode::Register(reg);

  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto cppGen = graph.AddNode<CodeGenNode>(ImVec2{300.0f, -50.0f});
  auto javaGen = graph.AddNode<CodeGenNode>(ImVec2{300.0f, 50.0f});
  auto exportNode = graph.AddNode<ExportNode>(ImVec2{300.0f, 150.0f});
  cppGen->Logic().lang = Language::Cpp;
  javaGen->Logic().lang = Language::Java;
  exportNode->Logic().lang = Language::Python;
  cppGen->inPin("in")->createLink(stage->outPin("filter"));
  javaGen->inPin("in")->createLink(stage->outPin("filter"));
  exportNode->inPin("in")->createLink(stage->outPin("filter"));

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  EXPECT_EQ(restored.Links().size(), 3u);
  EXPECT_EQ(restored.Nodes().size(), 4u);
}

}  // namespace

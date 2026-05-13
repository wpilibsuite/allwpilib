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
#include "wpi/filterdesigner/nodes/ExportNodeLogic.hpp"

namespace {

using wpi::filterdesigner::BiquadStageNode;
using wpi::filterdesigner::CodeGenNode;
using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::ExportNode;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::Language;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;

void RegisterAll(NodeRegistry& reg) {
  BiquadStageNode::Register(reg);
  CodeGenNode::Register(reg);
  ExportNode::Register(reg);
}

TEST(M5SerializeTest, CodeGenParamsRoundTrip) {
  NodeRegistry reg;
  RegisterAll(reg);
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

TEST(M5SerializeTest, CodeGenEmptyVarNameDefaultsOnLoad) {
  NodeRegistry reg;
  RegisterAll(reg);
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

TEST(M5SerializeTest, ExportParamsRoundTrip) {
  NodeRegistry reg;
  RegisterAll(reg);
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

TEST(M5SerializeTest, BiquadStageToCodeGenLinkRoundTrips) {
  NodeRegistry reg;
  RegisterAll(reg);
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

TEST(M5SerializeTest, UnchainedBiquadStageFilterEmitsOwnSectionsOnly) {
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  stage->Logic().sampleRate = 1000.0;
  // Defaults design something sensible — exact section count isn't the
  // point; we just need a non-null combined filter to compare against.
  const auto* combined = stage->CombinedFilter();
  ASSERT_NE(combined, nullptr);
  const auto* own = stage->Logic().Filter();
  ASSERT_NE(own, nullptr);
  EXPECT_EQ(combined->sections.size(), own->sections.size());
  EXPECT_DOUBLE_EQ(combined->sampleRate, own->sampleRate);
}

TEST(M5SerializeTest, ChainedBiquadStagesEmitCumulativeCascade) {
  // The why-cascade-on-filter-pin demo: Stage A → Stage B → CodeGen exports
  // A+B, not just B. Mirrors the linear-chain tool's behavior.
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto stageA = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto stageB = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  stageA->Logic().sampleRate = 1000.0;
  stageB->Logic().sampleRate = 1000.0;
  stageB->inPin("in")->createLink(stageA->outPin("signal"));

  const auto* combinedB = stageB->CombinedFilter();
  ASSERT_NE(combinedB, nullptr);
  const auto* ownA = stageA->Logic().Filter();
  const auto* ownB = stageB->Logic().Filter();
  ASSERT_NE(ownA, nullptr);
  ASSERT_NE(ownB, nullptr);
  EXPECT_EQ(combinedB->sections.size(),
            ownA->sections.size() + ownB->sections.size());
  EXPECT_DOUBLE_EQ(combinedB->sampleRate, 1000.0);
}

TEST(M5SerializeTest, ChainedBiquadStagesCacheStablePointer) {
  // Pointer stability matters — downstream sinks may compare pointer
  // identity. Repeated calls without param/topology changes must return
  // the same pointer.
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto stageA = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto stageB = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  stageB->inPin("in")->createLink(stageA->outPin("signal"));

  const auto* first = stageB->CombinedFilter();
  const auto* second = stageB->CombinedFilter();
  EXPECT_EQ(first, second);
}

TEST(M5SerializeTest, SampleRateMismatchSurfacesAsCombinedError) {
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto stageA = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto stageB = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  stageA->Logic().sampleRate = 1000.0;
  stageB->Logic().sampleRate = 2000.0;  // mismatch
  stageB->inPin("in")->createLink(stageA->outPin("signal"));

  const auto* combined = stageB->CombinedFilter();
  EXPECT_EQ(combined, nullptr);
  EXPECT_NE(stageB->CombinedError().find("Sample rate"), std::string::npos);
}

TEST(M5SerializeTest, BiquadStageToMultipleSinksRoundTrips) {
  // Multi-language export off one Filter wire — the canonical "why
  // CodeGen-as-sink" example from the plan.
  NodeRegistry reg;
  RegisterAll(reg);
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

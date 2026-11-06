// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>

#include <ImNodeFlow.h>
#include <catch2/catch_test_macros.hpp>

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

TEST_CASE("CodeGenNodeSerializeTest ParamsRoundTrip", "[filterdesigner]") {
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
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<CodeGenNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().lang == Language::Python);
  CHECK(loaded->Logic().varName == "shooterFilter");
}

TEST_CASE("CodeGenNodeSerializeTest EmptyVarNameDefaultsOnLoad",
          "[filterdesigner]") {
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
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<CodeGenNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().varName == "filter");
}

TEST_CASE("CodeGenNodeSerializeTest BiquadStageToCodeGenLinkRoundTrips",
          "[filterdesigner]") {
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
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto links = restored.Links();
  REQUIRE(links.size() == 1u);
  CHECK(links[0].srcId == stageId);
  CHECK(links[0].dstId == codegenId);
  CHECK(links[0].srcPin == "filter");
  CHECK(links[0].dstPin == "in");
}

TEST_CASE("CodeGenNodeSerializeTest BiquadStageToMultipleSinksRoundTrips",
          "[filterdesigner]") {
  // Multi-language export off one Filter wire: two CodeGen sinks, no
  // multiplexing inside either.
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
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  CHECK(restored.Links().size() == 3u);
  CHECK(restored.Nodes().size() == 4u);
}

TEST_CASE("CodeGenNodeSerializeTest OutOfRangeLangKeepsDefault",
          "[filterdesigner]") {
  NodeRegistry reg;
  CodeGenNode::Register(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "CodeGen", "pos": [0, 0], "lang": 1e100}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<CodeGenNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().lang == Language::Cpp);
}

}  // namespace

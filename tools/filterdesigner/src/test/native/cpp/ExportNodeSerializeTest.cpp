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
#include "wpi/filterdesigner/nodes/ExportNode.hpp"
#include "wpi/filterdesigner/nodes/ExportNodeLogic.hpp"

namespace {

using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::ExportNode;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::Language;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;

TEST_CASE("ExportNodeSerializeTest ParamsRoundTrip", "[filterdesigner]") {
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
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<ExportNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().lang == Language::Java);
  CHECK(loaded->Logic().className == "ShooterFilter");
  CHECK(loaded->Logic().projectRoot == "/path/to/robot");
}

TEST_CASE("ExportNodeSerializeTest OutOfRangeLangKeepsDefault",
          "[filterdesigner]") {
  NodeRegistry reg;
  ExportNode::Register(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "Export", "pos": [0, 0], "lang": 2.5}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<ExportNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().lang == Language::Cpp);
}

}  // namespace

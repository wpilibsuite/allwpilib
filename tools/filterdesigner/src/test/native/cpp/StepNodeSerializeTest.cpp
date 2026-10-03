// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>

#include <ImNodeFlow.h>
#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
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

TEST_CASE("StepNodeSerializeTest ParamsRoundTrip", "[filterdesigner]") {
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
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<StepNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK_DOUBLE_EQ(loaded->Logic().sampleRate, 200.0);
  CHECK(loaded->Logic().length == 32);
  CHECK(loaded->Logic().startSample == 5);
}

TEST_CASE("StepNodeSerializeTest OutOfRangeIntFieldsKeepDefaults",
          "[filterdesigner]") {
  NodeRegistry reg;
  StepNode::Register(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "Step", "pos": [0, 0],
       "length": 1e100, "startSample": 2.5}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<StepNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().length == 200);
  CHECK(loaded->Logic().startSample == 0);
}

}  // namespace

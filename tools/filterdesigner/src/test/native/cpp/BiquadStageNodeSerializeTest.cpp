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
#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/BodePlotNode.hpp"
#include "wpi/filterdesigner/nodes/WpiLogSourceNode.hpp"

namespace {

using wpi::filterdesigner::BiquadStageNode;
using wpi::filterdesigner::BodePlotNode;
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

TEST_CASE("BiquadStageNodeSerializeTest ParamsRoundTrip", "[filterdesigner]") {
  NodeRegistry reg;
  RegisterAll(reg);

  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{50.0f, 100.0f});
  stage->Logic().sampleRate = 2500.0;
  stage->Logic().sampleRateAutoSync = false;
  stage->Logic().stage.kind = StageKind::HighPass;
  stage->Logic().stage.family = Family::Chebyshev1;
  stage->Logic().stage.order = 6;
  stage->Logic().stage.f1 = 250.0;
  stage->Logic().stage.passbandRippleDb = 1.5;

  int stageId = stage->GraphId();
  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<BiquadStageNode*>(restored.FindNodeById(stageId));
  REQUIRE(loaded != nullptr);
  CHECK_DOUBLE_EQ(loaded->Logic().sampleRate, 2500.0);
  CHECK_FALSE(loaded->Logic().sampleRateAutoSync);
  CHECK(loaded->Logic().stage.kind == StageKind::HighPass);
  CHECK(loaded->Logic().stage.family == Family::Chebyshev1);
  CHECK(loaded->Logic().stage.order == 6);
  CHECK_DOUBLE_EQ(loaded->Logic().stage.f1, 250.0);
  CHECK_DOUBLE_EQ(loaded->Logic().stage.passbandRippleDb, 1.5);
}

TEST_CASE("BiquadStageNodeSerializeTest AutoSampleRateFlagRoundTrips",
          "[filterdesigner]") {
  // The default for a freshly-added stage is auto=true; verify the flag
  // survives save/load and that the explicit-false case also round-trips.
  NodeRegistry reg;
  RegisterAll(reg);

  Graph graph;
  auto autoStage = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto manualStage = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  REQUIRE(autoStage->Logic().sampleRateAutoSync);  // default
  manualStage->Logic().sampleRateAutoSync = false;
  int autoId = autoStage->GraphId();
  int manualId = manualStage->GraphId();

  std::string json = SerializeGraph(graph);
  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loadedAuto =
      dynamic_cast<BiquadStageNode*>(restored.FindNodeById(autoId));
  auto* loadedManual =
      dynamic_cast<BiquadStageNode*>(restored.FindNodeById(manualId));
  REQUIRE(loadedAuto != nullptr);
  REQUIRE(loadedManual != nullptr);
  CHECK(loadedAuto->Logic().sampleRateAutoSync);
  CHECK_FALSE(loadedManual->Logic().sampleRateAutoSync);
}

TEST_CASE("BiquadStageNodeSerializeTest ToBodePlotLinkRoundTrips",
          "[filterdesigner]") {
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
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto links = restored.Links();
  REQUIRE(links.size() == 1u);
  CHECK(links[0].srcId == stageId);
  CHECK(links[0].dstId == bodeId);
  CHECK(links[0].srcPin == "filter");
  CHECK(links[0].dstPin == "in0");
}

TEST_CASE("BiquadStageNodeSerializeTest SignalPassthroughLinkRoundTrips",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterAll(reg);

  // WpiLogSource → BiquadStage(signal) → next BiquadStage(in). Canonical
  // chain shape; verifies pin-name resolution for both BiquadStage outputs.
  Graph graph;
  auto src = graph.AddNode<WpiLogSourceNode>(ImVec2{0.0f, 0.0f});
  auto a = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  auto b = graph.AddNode<BiquadStageNode>(ImVec2{400.0f, 0.0f});
  a->inPin("in")->createLink(src->outPin("out"));
  b->inPin("in")->createLink(a->outPin("signal"));

  std::string json = SerializeGraph(graph);
  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(restored.Links().size() == 2u);

  bool foundSrcToA = false;
  bool foundAToB = false;
  for (const auto& link : restored.Links()) {
    if (link.srcPin == "out" && link.dstPin == "in") {
      foundSrcToA = true;
    } else if (link.srcPin == "signal" && link.dstPin == "in") {
      foundAToB = true;
    }
  }
  CHECK(foundSrcToA);
  CHECK(foundAToB);
}

TEST_CASE(
    "BiquadStageNodeSerializeTest MultiStageSignalPassThroughActuallyFilters",
    "[filterdesigner]") {
  // Two stages chained on the Signal pass-through: each Filtered() keeps the
  // length and changes the values.
  using wpi::filterdesigner::BiquadStageNodeLogic;
  using wpi::filterdesigner::Signal;

  Signal in;
  in.name = "ramp";
  in.sampleRate = 1000.0;
  in.quality = wpi::filterdesigner::GridQuality::Exact(1000.0);
  in.timestamps.resize(64);
  in.values.resize(64);
  for (int i = 0; i < 64; ++i) {
    in.timestamps[i] = static_cast<double>(i) / 1000.0;
    in.values[i] = static_cast<double>(i);
  }
  in.revision = 1;

  BiquadStageNodeLogic a;
  a.sampleRate = 1000.0;
  a.stage.f1 = 100.0;
  const Signal* afterA = a.Filtered(&in);
  REQUIRE(afterA != nullptr);
  REQUIRE(afterA->values.size() == in.values.size());

  BiquadStageNodeLogic b;
  b.sampleRate = 1000.0;
  b.stage.f1 = 50.0;
  const Signal* afterB = b.Filtered(afterA);
  REQUIRE(afterB != nullptr);
  REQUIRE(afterB->values.size() == afterA->values.size());

  // The two cascade outputs should differ at the back of the buffer where
  // the filter has had time to integrate — if Filtered() ever silently
  // forwarded its input, this would catch it.
  bool anyDifferent = false;
  for (std::size_t i = afterA->values.size() / 2; i < afterA->values.size();
       ++i) {
    if (afterA->values[i] != afterB->values[i]) {
      anyDifferent = true;
      break;
    }
  }
  UNSCOPED_INFO("Chained Filtered() should compose, not pass through");
  CHECK(anyDifferent);
}

TEST_CASE("BiquadStageNodeSerializeTest OutOfRangeIntFieldsKeepDefaults",
          "[filterdesigner]") {
  // Converting a double outside int's range is undefined, so a huge or
  // fractional value is refused before any cast and the default stands.
  NodeRegistry reg;
  RegisterAll(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "BiquadStage", "pos": [0, 0],
       "kind": 1e100, "family": -2.5, "order": 1e100, "taps": 2.5}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<BiquadStageNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().stage.kind == StageKind::LowPass);
  CHECK(loaded->Logic().stage.family == Family::Butterworth);
  CHECK(loaded->Logic().stage.order == 4);
  CHECK(loaded->Logic().stage.taps == 5);
}

}  // namespace

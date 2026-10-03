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
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#include "wpi/filterdesigner/nodes/CodeGenNode.hpp"
#include "wpi/filterdesigner/nodes/ImpulseNode.hpp"
#include "wpi/filterdesigner/nodes/ImpulseNodeLogic.hpp"

namespace {

using wpi::filterdesigner::BiquadStageNode;
using wpi::filterdesigner::CodeGenNode;
using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;

TEST_CASE("BiquadStageCascadeTest UnchainedStageFilterEmitsOwnSectionsOnly",
          "[filterdesigner]") {
  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  stage->Logic().sampleRate = 1000.0;
  // Defaults design something sensible — exact section count isn't the
  // point; we just need a non-null combined filter to compare against.
  const auto* combined = stage->CombinedFilter();
  REQUIRE(combined != nullptr);
  const auto* own = stage->Logic().Filter();
  REQUIRE(own != nullptr);
  CHECK(combined->sections.size() == own->sections.size());
  CHECK_DOUBLE_EQ(combined->sampleRate, own->sampleRate);
}

TEST_CASE("BiquadStageCascadeTest ChainedStagesEmitCumulativeCascade",
          "[filterdesigner]") {
  // Stage A → Stage B → CodeGen exports A+B, not just B; wiring earlier in
  // the Signal chain is how you ask for one stage alone.
  Graph graph;
  auto stageA = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto stageB = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  stageA->Logic().sampleRate = 1000.0;
  stageB->Logic().sampleRate = 1000.0;
  stageB->inPin("in")->createLink(stageA->outPin("signal"));

  const auto* combinedB = stageB->CombinedFilter();
  REQUIRE(combinedB != nullptr);
  const auto* ownA = stageA->Logic().Filter();
  const auto* ownB = stageB->Logic().Filter();
  REQUIRE(ownA != nullptr);
  REQUIRE(ownB != nullptr);
  CHECK(combinedB->sections.size() ==
        ownA->sections.size() + ownB->sections.size());
  CHECK_DOUBLE_EQ(combinedB->sampleRate, 1000.0);
}

TEST_CASE("BiquadStageCascadeTest ChainedStagesCacheStablePointer",
          "[filterdesigner]") {
  // Downstream sinks compare pointer identity, so repeated calls without a
  // param or topology change must return the same pointer.
  Graph graph;
  auto stageA = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto stageB = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  stageB->inPin("in")->createLink(stageA->outPin("signal"));

  const auto* first = stageB->CombinedFilter();
  const auto* second = stageB->CombinedFilter();
  CHECK(first == second);
}

TEST_CASE("BiquadStageCascadeTest SampleRateMismatchSurfacesAsCombinedError",
          "[filterdesigner]") {
  Graph graph;
  auto stageA = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto stageB = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  stageA->Logic().sampleRate = 1000.0;
  stageB->Logic().sampleRate = 2000.0;  // mismatch
  stageB->inPin("in")->createLink(stageA->outPin("signal"));

  const auto* combined = stageB->CombinedFilter();
  CHECK(combined == nullptr);
  CHECK(stageB->CombinedError().find("Sample rate") != std::string::npos);
}

TEST_CASE("BiquadStageCascadeTest CombinedFilterSurvivesSerializeDeserialize",
          "[filterdesigner]") {
  // A → B → CodeGen, saved and reloaded: the restored B's cumulative cascade
  // still has A's sections in front of its own.
  NodeRegistry reg;
  BiquadStageNode::Register(reg);
  CodeGenNode::Register(reg);

  Graph graph;
  auto stageA = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto stageB = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  auto codegen = graph.AddNode<CodeGenNode>(ImVec2{400.0f, 0.0f});
  stageA->Logic().sampleRate = 1000.0;
  stageB->Logic().sampleRate = 1000.0;
  stageB->inPin("in")->createLink(stageA->outPin("signal"));
  codegen->inPin("in")->createLink(stageB->outPin("filter"));
  int aId = stageA->GraphId();
  int bId = stageB->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* restoredA = dynamic_cast<BiquadStageNode*>(restored.FindNodeById(aId));
  auto* restoredB = dynamic_cast<BiquadStageNode*>(restored.FindNodeById(bId));
  REQUIRE(restoredA != nullptr);
  REQUIRE(restoredB != nullptr);
  const auto* combinedB = restoredB->CombinedFilter();
  REQUIRE(combinedB != nullptr);
  const auto* ownA = restoredA->Logic().Filter();
  const auto* ownB = restoredB->Logic().Filter();
  REQUIRE(ownA != nullptr);
  REQUIRE(ownB != nullptr);
  CHECK(combinedB->sections.size() ==
        ownA->sections.size() + ownB->sections.size());
}

TEST_CASE("BiquadStageCascadeTest NonBiquadUpstreamYieldsThisStageOnly",
          "[filterdesigner]") {
  // An upstream that isn't a BiquadStage leaves UpstreamStage() null, so the
  // cascade collapses to this stage's own sections.
  Graph graph;
  auto impulse =
      graph.AddNode<wpi::filterdesigner::ImpulseNode>(ImVec2{0.0f, 0.0f});
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  impulse->Logic().sampleRate = 1000.0;
  impulse->Logic().length = 64;
  stage->Logic().sampleRate = 1000.0;
  stage->inPin("in")->createLink(impulse->outPin("out"));

  const auto* combined = stage->CombinedFilter();
  REQUIRE(combined != nullptr);
  const auto* own = stage->Logic().Filter();
  REQUIRE(own != nullptr);
  // Same section count as the single stage — nothing got prepended from
  // the non-BiquadStage upstream.
  CHECK(combined->sections.size() == own->sections.size());
}

TEST_CASE("BiquadStageCascadeTest CycleGuardCatchesTwoNodeCycleWithoutCrashing",
          "[filterdesigner]") {
  // A.signal → B.in and B.signal → A.in. ImNodeFlow's same-parent guard
  // refuses a self-link but not a two-node cycle, which CombinedFilter would
  // otherwise recurse through unbounded; the depth guard turns that into a
  // nullptr and a cycle error.
  Graph graph;
  auto a = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto b = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  a->Logic().sampleRate = 1000.0;
  b->Logic().sampleRate = 1000.0;
  b->inPin("in")->createLink(a->outPin("signal"));
  a->inPin("in")->createLink(b->outPin("signal"));

  const auto* combined = a->CombinedFilter();
  CHECK(combined == nullptr);
  UNSCOPED_INFO("expected cycle-guard message, got: " << a->CombinedError());
  CHECK(a->CombinedError().find("cycle") != std::string::npos);
}

TEST_CASE("BiquadStageCascadeTest UpstreamErrorForReportsUnwiredAsEmpty",
          "[filterdesigner]") {
  // No link → empty string, which is how sinks tell an unwired input from a
  // wired-but-errored one.
  Graph graph;
  auto codegen = graph.AddNode<CodeGenNode>(ImVec2{0.0f, 0.0f});
  CHECK(BiquadStageNode::UpstreamErrorFor(codegen->inPin("in")).empty());
}

TEST_CASE("BiquadStageCascadeTest UpstreamErrorForReportsStageDesignError",
          "[filterdesigner]") {
  // A deliberately broken upstream: the helper surfaces its error string.
  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto codegen = graph.AddNode<CodeGenNode>(ImVec2{200.0f, 0.0f});
  // Negative sample rate fails the Filter() factory's pre-check.
  stage->Logic().sampleRate = -1.0;
  codegen->inPin("in")->createLink(stage->outPin("filter"));

  // Force the upstream to populate its error state.
  REQUIRE(stage->CombinedFilter() == nullptr);
  CHECK_FALSE(stage->CombinedError().empty());
  CHECK_FALSE(BiquadStageNode::UpstreamErrorFor(codegen->inPin("in")).empty());
}

TEST_CASE("BiquadStageCascadeTest RatesInsideTheDeadbandStillCombine",
          "[filterdesigner]") {
  // Auto-sync leaves a stage's rate alone while an input stays within 1% of
  // it, so two stages tracking one wobbling live source settle a little
  // apart. Both filter the signal happily under the 2% input tolerance, and
  // an exact comparison here used to withhold the Bode plot and codegen from
  // a chain that was working.
  Graph graph;
  auto stageA = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto stageB = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  stageA->Logic().sampleRate = 990.0;
  stageB->Logic().sampleRate = 1000.0;
  stageB->inPin("in")->createLink(stageA->outPin("signal"));

  const auto* combined = stageB->CombinedFilter();
  UNSCOPED_INFO(stageB->CombinedError());
  REQUIRE(combined != nullptr);
  UNSCOPED_INFO("the cascade is plotted at the downstream stage's rate");
  CHECK_DOUBLE_EQ(combined->sampleRate, 1000.0);
  const auto* own = stageB->Logic().Filter();
  REQUIRE(own != nullptr);
  CHECK(combined->sections.size() > own->sections.size());
}

TEST_CASE("BiquadStageCascadeTest RatesJustOutsideTheToleranceStillMismatch",
          "[filterdesigner]") {
  // 3% apart: past the 2% the input boundary accepts, so the cascade has to
  // keep refusing it.
  Graph graph;
  auto stageA = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto stageB = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  stageA->Logic().sampleRate = 970.0;
  stageB->Logic().sampleRate = 1000.0;
  stageB->inPin("in")->createLink(stageA->outPin("signal"));

  CHECK(stageB->CombinedFilter() == nullptr);
  CHECK(stageB->CombinedError().find("Sample rate") != std::string::npos);
}

}  // namespace

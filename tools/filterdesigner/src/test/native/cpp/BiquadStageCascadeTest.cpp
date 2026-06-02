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

TEST(BiquadStageCascadeTest, UnchainedStageFilterEmitsOwnSectionsOnly) {
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

TEST(BiquadStageCascadeTest, ChainedStagesEmitCumulativeCascade) {
  // The why-cascade-on-filter-pin demo: Stage A → Stage B → CodeGen exports
  // A+B, not just B. Users opt in to "just this stage" by wiring earlier in
  // the Signal chain.
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

TEST(BiquadStageCascadeTest, ChainedStagesCacheStablePointer) {
  // Pointer stability matters — downstream sinks may compare pointer
  // identity. Repeated calls without param/topology changes must return
  // the same pointer.
  Graph graph;
  auto stageA = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto stageB = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  stageB->inPin("in")->createLink(stageA->outPin("signal"));

  const auto* first = stageB->CombinedFilter();
  const auto* second = stageB->CombinedFilter();
  EXPECT_EQ(first, second);
}

TEST(BiquadStageCascadeTest, SampleRateMismatchSurfacesAsCombinedError) {
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

TEST(BiquadStageCascadeTest, CombinedFilterSurvivesSerializeDeserialize) {
  // Two-stage cascade A → B → CodeGen, save, reload, then pull
  // CombinedFilter() on the restored stage B and confirm the cumulative
  // section count matches A + B. Pairs with the topology round-trip test
  // to verify the math survives serialize too.
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto* restoredA = dynamic_cast<BiquadStageNode*>(restored.FindNodeById(aId));
  auto* restoredB = dynamic_cast<BiquadStageNode*>(restored.FindNodeById(bId));
  ASSERT_NE(restoredA, nullptr);
  ASSERT_NE(restoredB, nullptr);
  const auto* combinedB = restoredB->CombinedFilter();
  ASSERT_NE(combinedB, nullptr);
  const auto* ownA = restoredA->Logic().Filter();
  const auto* ownB = restoredB->Logic().Filter();
  ASSERT_NE(ownA, nullptr);
  ASSERT_NE(ownB, nullptr);
  EXPECT_EQ(combinedB->sections.size(),
            ownA->sections.size() + ownB->sections.size());
}

TEST(BiquadStageCascadeTest, NonBiquadUpstreamYieldsThisStageOnly) {
  // ImpulseSource → BiquadStage → CodeGen. The Impulse-style source isn't a
  // BiquadStage, so the dynamic_cast in UpstreamStage() must return null
  // and the cascade collapses to just this stage's sections. Closes the
  // "dynamic_cast nullptr branch is uncovered" gap.
  Graph graph;
  auto impulse =
      graph.AddNode<wpi::filterdesigner::ImpulseNode>(ImVec2{0.0f, 0.0f});
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  impulse->Logic().sampleRate = 1000.0;
  impulse->Logic().length = 64;
  stage->Logic().sampleRate = 1000.0;
  stage->inPin("in")->createLink(impulse->outPin("out"));

  const auto* combined = stage->CombinedFilter();
  ASSERT_NE(combined, nullptr);
  const auto* own = stage->Logic().Filter();
  ASSERT_NE(own, nullptr);
  // Same section count as the single stage — nothing got prepended from
  // the non-BiquadStage upstream.
  EXPECT_EQ(combined->sections.size(), own->sections.size());
}

TEST(BiquadStageCascadeTest, CycleGuardCatchesTwoNodeCycleWithoutCrashing) {
  // A.signal → B.in and B.signal → A.in. ImNodeFlow refuses same-node
  // links so a length-1 self-loop can't be wired through the public API,
  // but a two-node cycle slips past the same-parent guard. Without the
  // depth guard, CombinedFilter() would recurse unbounded between A and B
  // and stack-overflow on the per-frame walk; the guard turns that into a
  // nullptr + cycle error. Graph-level cycle detection (TopologyTest) is
  // the primary defense in production; this test pins the per-stage
  // backstop that fires when callers walk upstream directly.
  Graph graph;
  auto a = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto b = graph.AddNode<BiquadStageNode>(ImVec2{200.0f, 0.0f});
  a->Logic().sampleRate = 1000.0;
  b->Logic().sampleRate = 1000.0;
  b->inPin("in")->createLink(a->outPin("signal"));
  a->inPin("in")->createLink(b->outPin("signal"));

  const auto* combined = a->CombinedFilter();
  EXPECT_EQ(combined, nullptr);
  EXPECT_NE(a->CombinedError().find("cycle"), std::string::npos)
      << "expected cycle-guard message, got: " << a->CombinedError();
}

TEST(BiquadStageCascadeTest, UpstreamErrorForReportsUnwiredAsEmpty) {
  // Helper that sinks call to differentiate "no input wired" from "input
  // wired but errored": no link → empty string.
  Graph graph;
  auto codegen = graph.AddNode<CodeGenNode>(ImVec2{0.0f, 0.0f});
  EXPECT_TRUE(BiquadStageNode::UpstreamErrorFor(codegen->inPin("in")).empty());
}

TEST(BiquadStageCascadeTest, UpstreamErrorForReportsStageDesignError) {
  // Wire a deliberately broken BiquadStage to the sink and verify the
  // helper surfaces the upstream's error string.
  Graph graph;
  auto stage = graph.AddNode<BiquadStageNode>(ImVec2{0.0f, 0.0f});
  auto codegen = graph.AddNode<CodeGenNode>(ImVec2{200.0f, 0.0f});
  // Negative sample rate fails the Filter() factory's pre-check.
  stage->Logic().sampleRate = -1.0;
  codegen->inPin("in")->createLink(stage->outPin("filter"));

  // Force the upstream to populate its error state.
  ASSERT_EQ(stage->CombinedFilter(), nullptr);
  EXPECT_FALSE(stage->CombinedError().empty());
  EXPECT_FALSE(BiquadStageNode::UpstreamErrorFor(codegen->inPin("in")).empty());
}

}  // namespace

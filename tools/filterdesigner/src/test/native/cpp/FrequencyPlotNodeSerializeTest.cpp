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
#include "wpi/filterdesigner/model/Spectrum.hpp"
#include "wpi/filterdesigner/nodes/FrequencyPlotNode.hpp"
#include "wpi/filterdesigner/nodes/FrequencyPlotNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/ImpulseNode.hpp"

namespace {

using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::FrequencyPlotNode;
using wpi::filterdesigner::FrequencyPlotNodeLogic;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::ImpulseNode;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;

TEST_CASE("FrequencyPlotNodeSerializeTest ParamsRoundTrip",
          "[filterdesigner]") {
  NodeRegistry reg;
  FrequencyPlotNode::Register(reg);

  Graph graph;
  auto plot = graph.AddNode<FrequencyPlotNode>(ImVec2{50.0f, 60.0f});
  plot->Logic().autoscale = false;
  plot->Logic().showLegend = false;
  plot->Logic().logFrequency = true;
  plot->Logic().plotWidth = 480.0f;
  plot->Logic().plotHeight = 280.0f;
  int id = plot->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<FrequencyPlotNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK_FALSE(loaded->Logic().autoscale);
  CHECK_FALSE(loaded->Logic().showLegend);
  CHECK(loaded->Logic().logFrequency);
  CHECK_FLOAT_EQ(loaded->Logic().plotWidth, 480.0f);
  CHECK_FLOAT_EQ(loaded->Logic().plotHeight, 280.0f);
}

TEST_CASE("FrequencyPlotNodeSerializeTest SizeClampedToMinOnLoad",
          "[filterdesigner]") {
  NodeRegistry reg;
  FrequencyPlotNode::Register(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FrequencyPlot", "pos": [0, 0],
       "plotWidth": 1.0, "plotHeight": 1.0}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<FrequencyPlotNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  CHECK_FLOAT_EQ(loaded->Logic().plotWidth,
                 FrequencyPlotNodeLogic::kMinPlotWidth);
  CHECK_FLOAT_EQ(loaded->Logic().plotHeight,
                 FrequencyPlotNodeLogic::kMinPlotHeight);
}

TEST_CASE("FrequencyPlotNodeSerializeTest OutOfFloatRangeSizeKeepsDefault",
          "[filterdesigner]") {
  // Narrowing 1e100 to a float is undefined and yields an infinity, which
  // the lower-bound clamp happily keeps and ImPlot is then asked to draw at.
  NodeRegistry reg;
  FrequencyPlotNode::Register(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FrequencyPlot", "pos": [0, 0],
       "plotWidth": 1e100, "plotHeight": 1e999}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<FrequencyPlotNode*>(restored.FindNodeById(1));
  REQUIRE(loaded != nullptr);
  FrequencyPlotNodeLogic fresh;
  CHECK_FLOAT_EQ(loaded->Logic().plotWidth, fresh.plotWidth);
  CHECK_FLOAT_EQ(loaded->Logic().plotHeight, fresh.plotHeight);
}

TEST_CASE("FrequencyPlotNodeSerializeTest ConsumesUpstreamSignalSpectrum",
          "[filterdesigner]") {
  // The math draw() would run, on the wire content it would run it against —
  // Spectrum::Compute over the upstream Impulse's Signal, no ImGui needed.
  NodeRegistry reg;
  ImpulseNode::Register(reg);
  FrequencyPlotNode::Register(reg);

  Graph graph;
  auto impulse = graph.AddNode<ImpulseNode>(ImVec2{0.0f, 0.0f});
  auto fp = graph.AddNode<FrequencyPlotNode>(ImVec2{200.0f, 0.0f});
  impulse->Logic().sampleRate = 1000.0;
  impulse->Logic().length = 256;
  fp->inPin("in0")->createLink(impulse->outPin("out"));

  const auto* sig = impulse->Logic().Signal();
  REQUIRE(sig != nullptr);
  auto spec = wpi::filterdesigner::Spectrum::Compute(
      sig->values, sig->sampleRate,
      wpi::filterdesigner::SpectrumMode::kTransient);
  REQUIRE(spec.has_value());
  CHECK_FALSE(spec->frequencies.empty());
  CHECK(spec->frequencies.size() == spec->magnitudesDb.size());
}

}  // namespace

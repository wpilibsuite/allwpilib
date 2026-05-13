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

TEST(FrequencyPlotNodeSerializeTest, ParamsRoundTrip) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<FrequencyPlotNode*>(restored.FindNodeById(id));
  ASSERT_NE(loaded, nullptr);
  EXPECT_FALSE(loaded->Logic().autoscale);
  EXPECT_FALSE(loaded->Logic().showLegend);
  EXPECT_TRUE(loaded->Logic().logFrequency);
  EXPECT_FLOAT_EQ(loaded->Logic().plotWidth, 480.0f);
  EXPECT_FLOAT_EQ(loaded->Logic().plotHeight, 280.0f);
}

TEST(FrequencyPlotNodeSerializeTest, SizeClampedToMinOnLoad) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  auto* loaded = dynamic_cast<FrequencyPlotNode*>(restored.FindNodeById(1));
  ASSERT_NE(loaded, nullptr);
  EXPECT_FLOAT_EQ(loaded->Logic().plotWidth,
                  FrequencyPlotNodeLogic::kMinPlotWidth);
  EXPECT_FLOAT_EQ(loaded->Logic().plotHeight,
                  FrequencyPlotNodeLogic::kMinPlotHeight);
}

TEST(FrequencyPlotNodeSerializeTest, ConsumesUpstreamSignalSpectrum) {
  // Verifies the math the FrequencyPlot's draw() runs against its upstream
  // signal, without needing an ImGui context. Impulse → FrequencyPlot in
  // the canvas would call Spectrum::Compute every frame; verify it on the
  // Impulse's Signal directly so we know the wire content is in the shape
  // the plot expects.
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
  ASSERT_NE(sig, nullptr);
  auto spec =
      wpi::filterdesigner::Spectrum::Compute(sig->values, sig->sampleRate);
  ASSERT_TRUE(spec.has_value());
  EXPECT_FALSE(spec->frequencies.empty());
  EXPECT_EQ(spec->frequencies.size(), spec->magnitudesDb.size());
}

}  // namespace

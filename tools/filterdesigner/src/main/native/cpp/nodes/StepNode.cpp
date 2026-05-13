// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/StepNode.hpp"

#include <algorithm>
#include <memory>
#include <utility>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <imgui.h>
#endif

namespace wpi::filterdesigner {

StepNode::StepNode() : m_logic(std::make_unique<StepNodeLogic>()) {
  setTitle("Step");
  setStyle(ImFlow::NodeStyle::green());
  auto* logic = m_logic.get();
  addOUT<const wpi::filterdesigner::Signal*>("out")->behaviour(
      [logic] { return logic->Signal(); });
}

StepNode::~StepNode() = default;

void StepNode::SerializeParams(wpi::util::json& obj) const {
  obj["sampleRate"] = m_logic->sampleRate;
  obj["length"] = std::clamp(m_logic->length, StepNodeLogic::kMinLength,
                             StepNodeLogic::kMaxLength);
  // Mirror StepNodeLogic::Signal's runtime clamp ([0, length-1]) so the
  // saved value can't read back as an out-of-range int from a hand-edited
  // file. The logic still clamps at use-time as a belt-and-braces guard.
  int savedStart = std::clamp(
      m_logic->startSample, 0,
      std::max(0, std::clamp(m_logic->length, StepNodeLogic::kMinLength,
                             StepNodeLogic::kMaxLength) -
                      1));
  obj["startSample"] = savedStart;
}

void StepNode::DeserializeParams(const wpi::util::json& obj) {
  if (const auto* p = obj.lookup("sampleRate"); p && p->is_number()) {
    m_logic->sampleRate = p->get_number();
  }
  if (const auto* p = obj.lookup("length"); p && p->is_number()) {
    int v = static_cast<int>(p->get_number());
    m_logic->length =
        std::clamp(v, StepNodeLogic::kMinLength, StepNodeLogic::kMaxLength);
  }
  if (const auto* p = obj.lookup("startSample"); p && p->is_number()) {
    int v = static_cast<int>(p->get_number());
    // Clamp into [0, length-1] symmetrically with length's loader-side
    // clamp; the logic clamps again at use-time but having the field hold
    // a sane value matches what `length` does.
    m_logic->startSample = std::clamp(v, 0, std::max(0, m_logic->length - 1));
  }
}

void StepNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "Step";
  entry.menuLabel = "Step";
  entry.menuCategory = "Sources";
  entry.outputTypes.emplace_back(typeid(const wpi::filterdesigner::Signal*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<StepNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void StepNode::draw() {
  const float kItemWidth = 160.0f;
  ImGui::SetNextItemWidth(kItemWidth);
  ImGui::InputDouble("Sample rate (Hz)", &m_logic->sampleRate, 0.0, 0.0,
                     "%.3f");
  ImGui::SetNextItemWidth(kItemWidth);
  if (ImGui::InputInt("Length (samples)", &m_logic->length)) {
    m_logic->length = std::clamp(m_logic->length, StepNodeLogic::kMinLength,
                                 StepNodeLogic::kMaxLength);
  }
  ImGui::SetNextItemWidth(kItemWidth);
  ImGui::InputInt("Step at sample", &m_logic->startSample);
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void StepNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner

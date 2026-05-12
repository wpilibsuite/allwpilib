// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/ImpulseNode.hpp"

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

ImpulseNode::ImpulseNode() : m_logic(std::make_unique<ImpulseNodeLogic>()) {
  setTitle("Impulse");
  setStyle(ImFlow::NodeStyle::green());
  auto* logic = m_logic.get();
  addOUT<const wpi::filterdesigner::Signal*>("out")->behaviour(
      [logic] { return logic->Signal(); });
}

ImpulseNode::~ImpulseNode() = default;

void ImpulseNode::SerializeParams(wpi::util::json& obj) const {
  obj["sampleRate"] = m_logic->sampleRate;
  obj["length"] = m_logic->length;
}

void ImpulseNode::DeserializeParams(const wpi::util::json& obj) {
  if (const auto* p = obj.lookup("sampleRate"); p && p->is_number()) {
    m_logic->sampleRate = p->get_number();
  }
  if (const auto* p = obj.lookup("length"); p && p->is_number()) {
    int v = static_cast<int>(p->get_number());
    m_logic->length =
        std::clamp(v, ImpulseNodeLogic::kMinLength, ImpulseNodeLogic::kMaxLength);
  }
}

void ImpulseNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "Impulse";
  entry.menuLabel = "Impulse";
  entry.menuCategory = "Sources";
  entry.outputTypes.emplace_back(typeid(const wpi::filterdesigner::Signal*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<ImpulseNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void ImpulseNode::draw() {
  const float kItemWidth = 160.0f;
  ImGui::SetNextItemWidth(kItemWidth);
  ImGui::InputDouble("Sample rate (Hz)", &m_logic->sampleRate, 0.0, 0.0, "%.3f");
  ImGui::SetNextItemWidth(kItemWidth);
  if (ImGui::InputInt("Length (samples)", &m_logic->length)) {
    m_logic->length = std::clamp(m_logic->length, ImpulseNodeLogic::kMinLength,
                                 ImpulseNodeLogic::kMaxLength);
  }
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void ImpulseNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner

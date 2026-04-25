// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/GraphEditor.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <memory>
#include <string>

#include <ImNodeFlow.h>

namespace wpi::filterdesigner {

namespace {

// Two throwaway debug nodes that exist only to prove the spike. They will be
// deleted in milestone 3 once the real source/filter/sink nodes land.

class DebugIntSourceNode final : public ImFlow::BaseNode {
 public:
  DebugIntSourceNode() {
    setTitle("Debug Int Source");
    setStyle(ImFlow::NodeStyle::green());
    addOUT<int>("out")->behaviour([this] { return m_value; });
  }

  void draw() override {
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderInt("##value", &m_value, -100, 100);
  }

 private:
  int m_value = 42;
};

class DebugIntDisplayNode final : public ImFlow::BaseNode {
 public:
  DebugIntDisplayNode() {
    setTitle("Debug Int Display");
    setStyle(ImFlow::NodeStyle::cyan());
    addIN<int>("in", 0, ImFlow::ConnectionFilter::SameType());
  }

  void draw() override {
    int v = getInVal<int>("in");
    ImGui::Text("value = %d", v);
  }
};

}  // namespace

GraphEditor::GraphEditor()
    : m_editor(std::make_unique<ImFlow::ImNodeFlow>("FilterDesignerGraph")) {
  auto src = m_editor->addNode<DebugIntSourceNode>(ImVec2(40.0f, 60.0f));
  auto dst = m_editor->addNode<DebugIntDisplayNode>(ImVec2(280.0f, 60.0f));

  ImFlow::Pin* outPin = src->outPin("out");
  ImFlow::Pin* inPin = dst->inPin("in");
  if (outPin && inPin) {
    inPin->createLink(outPin);
  }
}

GraphEditor::~GraphEditor() = default;

void GraphEditor::Display() {
  m_editor->update();
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS

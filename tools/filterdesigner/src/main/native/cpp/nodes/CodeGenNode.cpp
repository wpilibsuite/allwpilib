// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/CodeGenNode.hpp"

#include <memory>
#include <utility>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <cstdio>
#include <string>

#include <imgui.h>
#endif

namespace wpi::filterdesigner {

namespace {
#ifndef RUNNING_FILTERDESIGNER_TESTS
constexpr const char* kLangLabels[] = {"C++", "Java", "Python"};
#endif
}  // namespace

CodeGenNode::CodeGenNode() : m_logic(std::make_unique<CodeGenNodeLogic>()) {
  setTitle("Code Gen");
  setStyle(ImFlow::NodeStyle::red());
  addIN<const wpi::filterdesigner::DesignedFilter*>(
      "in", nullptr, ImFlow::ConnectionFilter::SameType());
}

CodeGenNode::~CodeGenNode() = default;

void CodeGenNode::SerializeParams(wpi::util::json& obj) const {
  obj["lang"] = static_cast<int>(m_logic->lang);
  obj["varName"] = m_logic->varName;
}

void CodeGenNode::DeserializeParams(const wpi::util::json& obj) {
  if (const auto* p = obj.lookup("lang"); p && p->is_number()) {
    int v = static_cast<int>(p->get_number());
    if (v >= 0 && v <= static_cast<int>(Language::Python)) {
      m_logic->lang = static_cast<Language>(v);
    }
  }
  if (const auto* p = obj.lookup("varName"); p && p->is_string()) {
    m_logic->varName = p->get_string();
    if (m_logic->varName.empty()) {
      m_logic->varName = "filter";
    }
  }
}

void CodeGenNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "CodeGen";
  entry.menuLabel = "Code Gen";
  entry.menuCategory = "Export";
  entry.inputTypes.emplace_back(
      typeid(const wpi::filterdesigner::DesignedFilter*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<CodeGenNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void CodeGenNode::draw() {
  const float kItemWidth = 220.0f;

  int langIdx = static_cast<int>(m_logic->lang);
  ImGui::SetNextItemWidth(kItemWidth);
  if (ImGui::Combo("Language", &langIdx, kLangLabels,
                   IM_ARRAYSIZE(kLangLabels))) {
    m_logic->lang = static_cast<Language>(langIdx);
  }

  char varBuf[128];
  std::snprintf(varBuf, sizeof(varBuf), "%s", m_logic->varName.c_str());
  ImGui::SetNextItemWidth(kItemWidth);
  if (ImGui::InputText("Variable", varBuf, sizeof(varBuf))) {
    m_logic->varName = varBuf;
    if (m_logic->varName.empty()) {
      m_logic->varName = "filter";
    }
  }

  const DesignedFilter* filter = getInVal<const DesignedFilter*>("in");
  // Regenerate every frame — EmitCode is O(N sections) and cheap; the
  // displayed text needs to follow the upstream cascade without explicit
  // change tracking.
  std::string code = m_logic->Generate(filter);
  bool haveCode = !code.empty();

  ImGui::BeginDisabled(!haveCode);
  if (ImGui::Button("Copy to clipboard")) {
    ImGui::SetClipboardText(code.c_str());
  }
  ImGui::EndDisabled();

  if (!haveCode) {
    ImGui::TextDisabled("Connect a Filter to display code.");
    return;
  }

  // InputTextMultiline needs a mutable buffer even in read-only mode.
  // Hand it `code.data()` directly; the buffer size is fixed by the
  // ReadOnly flag.
  ImGui::InputTextMultiline("##code", code.data(), code.size() + 1,
                            ImVec2{kItemWidth, 200.0f},
                            ImGuiInputTextFlags_ReadOnly);
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void CodeGenNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner

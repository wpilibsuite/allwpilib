// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/CodeGenNode.hpp"

#include <memory>
#include <utility>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/JsonInt.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <algorithm>
#include <cstdio>
#include <string>

#include <imgui.h>

#include "wpi/filterdesigner/graph/Topology.hpp"
#include "wpi/filterdesigner/nodes/StatusText.hpp"
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
  if (auto v = ReadIntField(obj, "lang");
      v && *v >= 0 && *v <= static_cast<int>(Language::Python)) {
    m_logic->lang = static_cast<Language>(*v);
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
  if (DrawCycleBannerIfAny(this)) {
    return;
  }

  const float kItemWidth = 220.0f;
  // The code box grows to its widest line up to this, then scrolls. A section
  // line carries five 17-significant-digit doubles, so at the default font it
  // runs to roughly four times kItemWidth.
  constexpr float kMaxCodeWidth = 720.0f;
  constexpr float kMaxCodeHeight = 320.0f;

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
  // Every frame: EmitCode is O(sections), and the text has to follow the
  // upstream cascade.
  std::string code = m_logic->Generate(filter);
  bool haveCode = !code.empty();

  ImGui::BeginDisabled(!haveCode);
  if (ImGui::Button("Copy to clipboard")) {
    ImGui::SetClipboardText(code.c_str());
  }
  ImGui::EndDisabled();

  if (!haveCode) {
    // Or a wired-but-errored upstream renders as "Connect a Filter…".
    std::string upstreamErr = BiquadStageNode::UpstreamErrorFor(inPin("in"));
    if (!upstreamErr.empty()) {
      DrawStatusText(kStatusErrorColor, "Upstream: " + upstreamErr);
    } else {
      ImGui::TextDisabled("Connect a Filter to display code.");
    }
    return;
  }

  // InputTextMultiline neither wraps nor sizes to its text, so a box fixed at
  // kItemWidth showed the first few tokens of each line. Fit it to the code,
  // capped so a long cascade doesn't take the canvas over; past the cap the
  // box scrolls.
  const ImGuiStyle& style = ImGui::GetStyle();
  const ImVec2 textSize = ImGui::CalcTextSize(code.c_str());
  const ImVec2 boxSize{
      std::clamp(textSize.x + 2.0f * style.FramePadding.x + style.ScrollbarSize,
                 kItemWidth, kMaxCodeWidth),
      std::clamp(textSize.y + 2.0f * style.FramePadding.y + style.ScrollbarSize,
                 4.0f * ImGui::GetTextLineHeight(), kMaxCodeHeight)};

  // InputTextMultiline needs a mutable buffer even read-only.
  ImGui::InputTextMultiline("##code", code.data(), code.size() + 1, boxSize,
                            ImGuiInputTextFlags_ReadOnly);
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void CodeGenNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner

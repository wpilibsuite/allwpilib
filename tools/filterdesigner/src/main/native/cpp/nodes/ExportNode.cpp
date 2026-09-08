// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/ExportNode.hpp"

#include <memory>
#include <utility>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/codegen/Export.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/JsonInt.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#include "wpi/gui/portable-file-dialogs.h"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <cstdio>
#include <filesystem>
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

ExportNode::ExportNode() : m_logic(std::make_unique<ExportNodeLogic>()) {
  setTitle("Export");
  setStyle(ImFlow::NodeStyle::red());
  addIN<const wpi::filterdesigner::DesignedFilter*>(
      "in", nullptr, ImFlow::ConnectionFilter::SameType());
}

ExportNode::~ExportNode() = default;

void ExportNode::SerializeParams(wpi::util::json& obj) const {
  obj["lang"] = static_cast<int>(m_logic->lang);
  obj["className"] = m_logic->className;
  obj["projectRoot"] = m_logic->projectRoot;
}

void ExportNode::DeserializeParams(const wpi::util::json& obj) {
  if (auto v = ReadIntField(obj, "lang");
      v && *v >= 0 && *v <= static_cast<int>(Language::Python)) {
    m_logic->lang = static_cast<Language>(*v);
  }
  if (const auto* p = obj.lookup("className"); p && p->is_string()) {
    m_logic->className = p->get_string();
  }
  if (const auto* p = obj.lookup("projectRoot"); p && p->is_string()) {
    m_logic->projectRoot = p->get_string();
  }
}

void ExportNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "Export";
  entry.menuLabel = "Export";
  entry.menuCategory = "Export";
  entry.inputTypes.emplace_back(
      typeid(const wpi::filterdesigner::DesignedFilter*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<ExportNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void ExportNode::PollFolderPicker() {
  if (!m_rootPicker || !m_rootPicker->ready(0)) {
    return;
  }
  std::string picked = m_rootPicker->result();
  m_rootPicker.reset();
  if (!picked.empty()) {
    m_logic->projectRoot = std::move(picked);
  }
}

void ExportNode::draw() {
  // Before the cycle gate below: a pending pfd dialog has to keep ticking
  // whatever the graph is doing.
  PollFolderPicker();
  if (DrawCycleBannerIfAny(this)) {
    return;
  }

  const float kItemWidth = 220.0f;

  int langIdx = static_cast<int>(m_logic->lang);
  ImGui::SetNextItemWidth(kItemWidth);
  if (ImGui::Combo("Language", &langIdx, kLangLabels,
                   IM_ARRAYSIZE(kLangLabels))) {
    m_logic->lang = static_cast<Language>(langIdx);
  }

  char nameBuf[128];
  std::snprintf(nameBuf, sizeof(nameBuf), "%s", m_logic->className.c_str());
  ImGui::SetNextItemWidth(kItemWidth);
  if (ImGui::InputText("Class name", nameBuf, sizeof(nameBuf))) {
    m_logic->className = nameBuf;
  }

  if (ImGui::Button("Project root...")) {
    if (!m_rootPicker) {
      m_rootPicker = std::make_unique<pfd::select_folder>(
          "Select WPILib project root",
          m_logic->projectRoot.empty() ? std::string{} : m_logic->projectRoot);
    }
  }
  if (m_logic->projectRoot.empty()) {
    ImGui::TextDisabled("(no project root selected)");
  } else {
    DrawStatusTextDisabled(m_logic->projectRoot);
  }

  const DesignedFilter* filter = getInVal<const DesignedFilter*>("in");
  bool haveFilter = filter && !filter->sections.empty();
  bool rootGiven = !m_logic->projectRoot.empty();
  bool canExport = haveFilter && rootGiven;

  if (haveFilter && rootGiven) {
    auto target = ResolveExportPath(std::filesystem::path{m_logic->projectRoot},
                                    m_logic->lang,
                                    NormalizeClassName(m_logic->className));
    DrawStatusTextDisabled("Will write: " + target.string());
  } else if (!haveFilter) {
    // Distinguish "no upstream wired" from "upstream wired but errored".
    std::string upstreamErr = BiquadStageNode::UpstreamErrorFor(inPin("in"));
    if (!upstreamErr.empty()) {
      DrawStatusText(kStatusErrorColor, "Upstream: " + upstreamErr);
    } else {
      ImGui::TextDisabled("Connect a Filter to enable export.");
    }
  } else if (!rootGiven) {
    ImGui::TextDisabled("Set a project root to enable export.");
  }

  ImGui::BeginDisabled(!canExport);
  if (ImGui::Button("Export")) {
    m_logic->Export(filter);
  }
  ImGui::EndDisabled();

  if (!m_logic->lastMessage.empty()) {
    DrawStatusText(m_logic->lastOk ? kStatusOkColor : kStatusErrorColor,
                   m_logic->lastMessage);
  }
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void ExportNode::PollFolderPicker() {}
void ExportNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner

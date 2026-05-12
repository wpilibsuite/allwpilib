// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/WpiLogSourceNode.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/gui/portable-file-dialogs.h"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <filesystem>

#include <imgui.h>
#endif

namespace wpi::filterdesigner {

WpiLogSourceNode::WpiLogSourceNode()
    : m_logic(std::make_unique<WpiLogSourceNodeLogic>()) {
  setTitle("WPILOG Source");
  setStyle(ImFlow::NodeStyle::green());
  // Capture the raw logic pointer; the unique_ptr keeps it alive for the
  // node's lifetime, and ImNodeFlow tears down links before the OutPin is
  // destroyed, so this can't outlive its target.
  auto* logic = m_logic.get();
  addOUT<const wpi::filterdesigner::Signal*>("out")->behaviour(
      [logic] { return logic->Signal(); });
}

WpiLogSourceNode::~WpiLogSourceNode() = default;

void WpiLogSourceNode::SerializeParams(wpi::util::json& obj) const {
  obj["logPath"] = m_logic->LogPath();
  obj["entry"] = m_logic->SelectedEntry();
}

void WpiLogSourceNode::DeserializeParams(const wpi::util::json& obj) {
  std::string path;
  std::string entry;
  if (const auto* p = obj.lookup("logPath"); p && p->is_string()) {
    path = p->get_string();
  }
  if (const auto* e = obj.lookup("entry"); e && e->is_string()) {
    entry = e->get_string();
  }
  m_logic->RestoreFromPath(path, entry);
}

void WpiLogSourceNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "WpiLogSource";
  entry.menuLabel = "WPILOG Source";
  entry.menuCategory = "Sources";
  entry.outputTypes.emplace_back(typeid(const wpi::filterdesigner::Signal*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<WpiLogSourceNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void WpiLogSourceNode::PollFileDialog() {
  if (!m_opener || !m_opener->ready(0)) {
    return;
  }
  auto result = m_opener->result();
  m_opener.reset();
  if (result.empty()) {
    return;
  }
  m_logic->OpenFile(result.front());
}

void WpiLogSourceNode::draw() {
  PollFileDialog();

  if (ImGui::Button("Open .wpilog...")) {
    if (!m_opener) {
      m_opener = std::make_unique<pfd::open_file>(
          "Select Data Log", "",
          std::vector<std::string>{"DataLog Files", "*.wpilog"});
    }
  }
  if (!m_logic->LogPath().empty()) {
    ImGui::SameLine();
    ImGui::TextUnformatted(
        std::filesystem::path{m_logic->LogPath()}.filename().string().c_str());
  }
  if (!m_logic->LoadError().empty()) {
    ImGui::TextColored(ImVec4{1.0f, 0.4f, 0.4f, 1.0f}, "%s",
                       m_logic->LoadError().c_str());
  }
  if (!m_logic->HasFile()) {
    ImGui::TextDisabled("No log loaded.");
    return;
  }

  auto entries = m_logic->Entries();
  ImGui::TextDisabled("Entries (%zu):", entries.size());

  ImGui::SetNextItemWidth(220.0f);
  const char* currentLabel =
      m_logic->SelectedEntry().empty() ? "<none>"
                                       : m_logic->SelectedEntry().c_str();
  if (ImGui::BeginCombo("##entry", currentLabel)) {
    for (const auto& entry : entries) {
      const bool selected = entry.name == m_logic->SelectedEntry();
      if (!entry.numeric) {
        ImGui::BeginDisabled();
        ImGui::Selectable(entry.label.c_str(), selected);
        ImGui::EndDisabled();
        continue;
      }
      if (ImGui::Selectable(entry.label.c_str(), selected)) {
        m_logic->SelectEntry(entry.name);
      }
    }
    ImGui::EndCombo();
  }
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void WpiLogSourceNode::PollFileDialog() {}
void WpiLogSourceNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner

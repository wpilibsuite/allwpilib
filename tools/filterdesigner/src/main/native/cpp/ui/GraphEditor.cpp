// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/GraphEditor.hpp"

#include <memory>
#include <string>
#include <utility>

#include "wpi/filterdesigner/graph/CreationPopup.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/graph/Serialize.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#include "wpi/filterdesigner/nodes/BodePlotNode.hpp"
#include "wpi/filterdesigner/nodes/CodeGenNode.hpp"
#include "wpi/filterdesigner/nodes/ExportNode.hpp"
#include "wpi/filterdesigner/nodes/FrequencyPlotNode.hpp"
#include "wpi/filterdesigner/nodes/ImpulseNode.hpp"
#include "wpi/filterdesigner/nodes/PoleZeroPlotNode.hpp"
#include "wpi/filterdesigner/nodes/StepNode.hpp"
#include "wpi/filterdesigner/nodes/TimePlotNode.hpp"
#include "wpi/filterdesigner/nodes/WpiLogSourceNode.hpp"
#include "wpi/gui/portable-file-dialogs.h"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <filesystem>
#include <vector>

#include <imgui.h>

#endif

namespace wpi::filterdesigner {

GraphEditor::GraphEditor(std::string_view saveDir)
    : m_registry(std::make_unique<NodeRegistry>()),
      m_graph(std::make_unique<Graph>()),
      m_defaultDir(saveDir) {
  // Register every node type the tool knows about. Order here controls
  // the menu ordering in the creation popup.
  WpiLogSourceNode::Register(*m_registry);
  ImpulseNode::Register(*m_registry);
  StepNode::Register(*m_registry);
  BiquadStageNode::Register(*m_registry);
  TimePlotNode::Register(*m_registry);
  FrequencyPlotNode::Register(*m_registry);
  BodePlotNode::Register(*m_registry);
  PoleZeroPlotNode::Register(*m_registry);
  CodeGenNode::Register(*m_registry);
  ExportNode::Register(*m_registry);

  m_creationPopup =
      std::make_unique<CreationPopup>(*m_graph, *m_registry);
  m_creationPopup->Attach();
}

GraphEditor::~GraphEditor() = default;

void GraphEditor::RequestAddNodeAtMouse() {
  m_creationPopup->RequestOpenAtMouse();
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void GraphEditor::RequestSave() {
  if (m_saver) {
    return;
  }
  m_saver = std::make_unique<pfd::save_file>(
      "Save Filter Designer Graph", m_defaultDir,
      std::vector<std::string>{"Filter Designer Graph", "*.fdsgn"});
}

void GraphEditor::RequestLoad() {
  if (m_opener) {
    return;
  }
  m_opener = std::make_unique<pfd::open_file>(
      "Open Filter Designer Graph", m_defaultDir,
      std::vector<std::string>{"Filter Designer Graph", "*.fdsgn"});
}

void GraphEditor::PollDialogs() {
  if (m_saver && m_saver->ready(0)) {
    std::string path = m_saver->result();
    m_saver.reset();
    if (!path.empty()) {
      // pfd's save dialog hands back exactly what the user typed; if they
      // omitted the extension, default it to .fdsgn so the file matches the
      // open-dialog filter on the next round-trip.
      if (std::filesystem::path{path}.extension().empty()) {
        path += ".fdsgn";
      }
      std::string err = SaveGraphToFile(path, *m_graph);
      if (err.empty()) {
        m_status = "Saved: " + path;
        m_lastPath = std::move(path);
      } else {
        m_status = std::move(err);
      }
    }
  }
  if (m_opener && m_opener->ready(0)) {
    auto results = m_opener->result();
    m_opener.reset();
    if (!results.empty()) {
      DeserializeResult dr =
          LoadGraphFromFile(results.front(), *m_graph, *m_registry);
      // DeserializeGraph calls Graph::Reset, which throws away the previous
      // ImFlow::ImNodeFlow instance and with it the right-click + drop-link
      // callbacks the CreationPopup registered. Re-attach so the menu keeps
      // working after a load, regardless of success — even on a parse error
      // the editor was reset before the failure was discovered.
      m_creationPopup->Attach();
      if (dr.ok()) {
        m_status = "Loaded: " + results.front();
        if (!dr.warnings.empty()) {
          m_status += " (" + std::to_string(dr.warnings.size()) + " warnings)";
        }
        m_lastPath = results.front();
      } else {
        m_status = "Load failed: " + dr.error;
      }
    }
  }
}

void GraphEditor::Display() {
  PollDialogs();

  if (ImGui::Button("Save")) {
    RequestSave();
  }
  ImGui::SameLine();
  if (ImGui::Button("Open")) {
    RequestLoad();
  }
  ImGui::SameLine();
  if (ImGui::Button("Add Node")) {
    RequestAddNodeAtMouse();
  }
  if (!m_status.empty()) {
    ImGui::SameLine();
    ImGui::TextDisabled("%s", m_status.c_str());
  }
  ImGui::Separator();

  m_graph->Update();
  m_creationPopup->DrawExternal();
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void GraphEditor::RequestSave() {}
void GraphEditor::RequestLoad() {}
void GraphEditor::PollDialogs() {}
void GraphEditor::Display() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner

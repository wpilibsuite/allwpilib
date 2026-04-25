// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/io/WpiLogLoaderView.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <imgui.h>

#include "wpi/gui/portable-file-dialogs.h"

namespace wpi::filterdesigner {

WpiLogLoaderView::WpiLogLoaderView() = default;
WpiLogLoaderView::~WpiLogLoaderView() = default;

void WpiLogLoaderView::PollFileDialog() {
  if (!m_opener || !m_opener->ready(0)) {
    return;
  }
  auto result = m_opener->result();
  m_opener.reset();
  if (result.empty()) {
    return;
  }
  auto& path = result.front();
  auto src = WpiLogSource::FromFile(path);
  if (!src) {
    m_loadError = "Failed to open: " + path;
    m_source.reset();
    m_selected.reset();
    m_loadedPath.clear();
    return;
  }
  m_loadError.clear();
  m_source = std::move(src);
  m_selected.reset();
  m_loadedPath = path;
}

void WpiLogLoaderView::Display() {
  PollFileDialog();

  if (ImGui::Button("Open .wpilog...")) {
    if (!m_opener) {
      m_opener = std::make_unique<pfd::open_file>(
          "Select Data Log", "",
          std::vector<std::string>{"DataLog Files", "*.wpilog"});
    }
  }
  if (!m_loadedPath.empty()) {
    ImGui::SameLine();
    ImGui::TextUnformatted(
        std::filesystem::path{m_loadedPath}.filename().string().c_str());
  }
  if (!m_loadError.empty()) {
    ImGui::TextColored(ImVec4{1.0f, 0.4f, 0.4f, 1.0f}, "%s",
                       m_loadError.c_str());
  }
  if (!m_source) {
    ImGui::TextDisabled("No log loaded.");
    return;
  }

  ImGui::Separator();
  ImGui::Text("Entries (%zu):", m_source->Entries().size());

  if (ImGui::BeginChild("entries", ImVec2{0, 0}, ImGuiChildFlags_Borders)) {
    for (const auto& entry : m_source->Entries()) {
      if (!entry.numeric) {
        ImGui::BeginDisabled();
        ImGui::Selectable(entry.label.c_str(), false);
        ImGui::EndDisabled();
        continue;
      }
      bool selected = m_selected.has_value() && m_selected->name == entry.name;
      if (ImGui::Selectable(entry.label.c_str(), selected)) {
        auto sig = m_source->LoadEntry(entry.name);
        if (sig) {
          m_selected = std::move(*sig);
          m_selected->revision = ++m_revisionCounter;
          m_loadError.clear();
        } else {
          m_loadError = "Failed to load entry: " + entry.name;
        }
      }
    }
  }
  ImGui::EndChild();
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS

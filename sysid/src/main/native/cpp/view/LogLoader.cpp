// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/view/LogLoader.h"

#include <algorithm>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <glass/support/DataLogReaderThread.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <portable-file-dialogs.h>
#include <wpi/SmallVector.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>

using namespace sysid;

LogLoader::LogLoader(glass::Storage& storage, wpi::Logger& logger) {}

LogLoader::~LogLoader() = default;

void LogLoader::Display() {
  if (ImGui::Button("Open data log file...")) {
    m_opener = std::make_unique<pfd::open_file>(
        "Select Data Log", "",
        std::vector<std::string>{"DataLog Files", "*.wpilog"});
  }

  // Handle opening the file
  if (m_opener && m_opener->ready(0)) {
    if (!m_opener->result().empty()) {
      m_filename = m_opener->result()[0];

      auto fileBuffer = wpi::MemoryBuffer::GetFile(m_filename);
      if (!fileBuffer) {
        ImGui::OpenPopup("Error");
        m_error = fmt::format("Could not open file: {}",
                              fileBuffer.error().message());
        return;
      }

      wpi::log::DataLogReader reader{std::move(*fileBuffer)};
      if (!reader.IsValid()) {
        ImGui::OpenPopup("Error");
        m_error = "Not a valid datalog file";
        return;
      }
      unload();
      m_reader =
          std::make_unique<glass::DataLogReaderThread>(std::move(reader));
      m_entryTree.clear();
    }
    m_opener.reset();
  }

  // Handle errors
  ImGui::SetNextWindowSize(ImVec2(480.f, 0.0f));
  if (ImGui::BeginPopupModal("Error")) {
    ImGui::PushTextWrapPos(0.0f);
    ImGui::TextUnformatted(m_error.c_str());
    ImGui::PopTextWrapPos();
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  if (!m_reader) {
    return;
  }

  // Summary info
  ImGui::TextUnformatted(fs::path{m_filename}.stem().string().c_str());
  ImGui::Text("%u records, %u entries%s", m_reader->GetNumRecords(),
              m_reader->GetNumEntries(),
              m_reader->IsDone() ? "" : " (working)");

  if (!m_reader->IsDone()) {
    return;
  }

  bool refilter = ImGui::InputText("Filter", &m_filter);

  // Display tree of entries
  if (m_entryTree.empty() || refilter) {
    RebuildEntryTree();
  }

  ImGui::BeginTable(
      "Entries", 2,
      ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp);
  ImGui::TableSetupColumn("Name");
  ImGui::TableSetupColumn("Type");
  // ImGui::TableSetupColumn("Metadata");
  ImGui::TableHeadersRow();
  DisplayEntryTree(m_entryTree);
  ImGui::EndTable();
}

void LogLoader::RebuildEntryTree() {
  m_entryTree.clear();
  wpi::SmallVector<std::string_view, 16> parts;
  m_reader->ForEachEntryName([&](const glass::DataLogReaderEntry& entry) {
    // only show double/float/string entries (TODO: support struct/protobuf)
    if (entry.type != "double" && entry.type != "float" &&
        entry.type != "string") {
      return;
    }

    // filter on name
    if (!m_filter.empty() && !wpi::contains_lower(entry.name, m_filter)) {
      return;
    }

    parts.clear();
    // split on first : if one is present
    auto [prefix, mainpart] = wpi::split(entry.name, ':');
    if (mainpart.empty() || wpi::contains(prefix, '/')) {
      mainpart = entry.name;
    } else {
      parts.emplace_back(prefix);
    }
    wpi::split(mainpart, parts, '/', -1, false);

    // ignore a raw "/" key
    if (parts.empty()) {
      return;
    }

    // get to leaf
    auto nodes = &m_entryTree;
    for (auto part : wpi::drop_back(std::span{parts.begin(), parts.end()})) {
      auto it =
          std::find_if(nodes->begin(), nodes->end(),
                       [&](const auto& node) { return node.name == part; });
      if (it == nodes->end()) {
        nodes->emplace_back(part);
        // path is from the beginning of the string to the end of the current
        // part; this works because part is a reference to the internals of
        // entry.name
        nodes->back().path.assign(
            entry.name.data(), part.data() + part.size() - entry.name.data());
        it = nodes->end() - 1;
      }
      nodes = &it->children;
    }

    auto it = std::find_if(nodes->begin(), nodes->end(), [&](const auto& node) {
      return node.name == parts.back();
    });
    if (it == nodes->end()) {
      nodes->emplace_back(parts.back());
      // no need to set path, as it's identical to entry.name
      it = nodes->end() - 1;
    }
    it->entry = &entry;
  });
}

static void EmitEntry(const std::string& name,
                      const glass::DataLogReaderEntry& entry) {
  ImGui::TableNextColumn();
  ImGui::Selectable(name.c_str());
  if (ImGui::BeginDragDropSource()) {
    auto entryPtr = &entry;
    ImGui::SetDragDropPayload(
        entry.type == "string" ? "DataLogEntryString" : "DataLogEntry",
        &entryPtr,
        sizeof(entryPtr));  // NOLINT
    ImGui::TextUnformatted(entry.name.data(),
                           entry.name.data() + entry.name.size());
    ImGui::EndDragDropSource();
  }
  ImGui::TableNextColumn();
  ImGui::TextUnformatted(entry.type.data(),
                         entry.type.data() + entry.type.size());
#if 0
  ImGui::TableNextColumn();
  ImGui::TextUnformatted(entry.metadata.data(),
                         entry.metadata.data() + entry.metadata.size());
#endif
}

void LogLoader::DisplayEntryTree(const std::vector<EntryTreeNode>& tree) {
  for (auto&& node : tree) {
    if (node.entry) {
      EmitEntry(node.name, *node.entry);
    }

    if (!node.children.empty()) {
      ImGui::TableNextColumn();
      bool open = ImGui::TreeNodeEx(node.name.c_str(),
                                    ImGuiTreeNodeFlags_SpanFullWidth);
      ImGui::TableNextColumn();
#if 0
      ImGui::TableNextColumn();
#endif
      if (open) {
        DisplayEntryTree(node.children);
        ImGui::TreePop();
      }
    }
  }
}

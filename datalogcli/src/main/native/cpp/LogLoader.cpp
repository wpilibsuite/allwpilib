// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LogLoader.h"

#include <memory>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#include "fmt/base.h"
#include "wpi/DataLogReader.h"

#include <glass/support/DataLogReaderThread.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>

using namespace datalogcli;

LogLoader::LogLoader(glass::Storage& storage, wpi::Logger& logger) {}

LogLoader::~LogLoader() = default;

void LogLoader::Load(std::string_view log_path) {

  // Handle opening the file
  std::error_code ec;
  auto buf = wpi::MemoryBuffer::GetFile(log_path, ec);
  if (ec) {
    m_error = fmt::format("Could not open file: {}", ec.message());
    return;
  }

  wpi::log::DataLogReader reader{std::move(buf)};
  if (!reader.IsValid()) {
    m_error = "Not a valid datalog file";
    return;
  }
  unload(); // release the actual file, we have the data in the reader now
  m_reader = std::make_unique<glass::DataLogReaderThread>(std::move(reader));
  m_entryTree.clear();

  // Handle Errors
  fmt::println("{}", m_error);

  if (!m_reader) {
    return;
  }

  // Summary info
  fmt::println("{}", fs::path{m_filename}.stem().string().c_str());
  fmt::println("%u records, %u entries%s", m_reader->GetNumRecords(),
              m_reader->GetNumEntries(),
              m_reader->IsDone() ? "" : " (working)");

  if (!m_reader->IsDone()) {
    return;
  }

  /*ImGui::BeginTable(
      "Entries", 2,
      ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp);
  ImGui::TableSetupColumn("Name");
  ImGui::TableSetupColumn("Type");
  // ImGui::TableSetupColumn("Metadata");
  ImGui::TableHeadersRow();
  DisplayEntryTree(m_entryTree);
  ImGui::EndTable();*/
}

std::vector<wpi::log::DataLogRecord> LogLoader::GetRecords(std::string_view field_name) {
  int entry_id{GetTargetEntryId(field_name)};
  std::vector<wpi::log::DataLogRecord> record_list{};

  if (entry_id == -1) {
    // didnt find a record with the name we want
    return record_list;
  }

  auto iter = m_reader->GetReader().begin();

  while(!m_reader->IsDone()) {
    if (iter->GetEntry() == entry_id) {
      // this is the one we want
      record_list.push_back(*iter);
    }
    iter++;
  }

  return record_list;
}

int LogLoader::GetTargetEntryId(std::string_view name) {
  // get first entry
  auto iter = m_reader->GetReader().begin();
  // is it what we want?
  if (iter->IsStart()) { // looking for a start record to get the entry ID
    wpi::log::StartRecordData* entryData{};
    iter->GetStartData(entryData);
    if (entryData->name.compare(name) != 0) {
      // this is it!
      return entryData->entry;
    }
  }
  return -1;
}

/*void LogLoader::RebuildEntryTree() {
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
}*/

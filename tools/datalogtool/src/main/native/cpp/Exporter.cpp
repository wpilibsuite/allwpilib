// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Exporter.hpp"

#include <stdint.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <format>
#include <functional>
#include <future>
#include <limits>
#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include "App.hpp"
#include "CsvExport.hpp"
#include "wpi/datalog/DataLogReaderThread.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/gui/portable-file-dialogs.h"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/SpanExtras.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/fs.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {
struct InputFile {
  explicit InputFile(std::unique_ptr<wpi::log::DataLogReaderThread> datalog);

  InputFile(std::string_view filename, std::string_view status)
      : filename{filename},
        stem{fs::path{filename}.stem().string()},
        status{status} {}

  ~InputFile();

  std::string filename;
  std::string stem;
  std::unique_ptr<wpi::log::DataLogReaderThread> datalog;
  std::string status;
  bool highlight = false;
};

struct Entry {
  explicit Entry(const wpi::log::StartRecordData& srd)
      : name{srd.name}, type{srd.type}, metadata{srd.metadata} {}

  std::string name;
  std::string type;
  std::string metadata;
  std::set<InputFile*> inputFiles;
  bool typeConflict = false;
  bool metadataConflict = false;
  bool selected = true;
};

struct EntryTreeNode {
  explicit EntryTreeNode(std::string_view name) : name{name} {}
  std::string name;  // name of just this node
  std::string path;  // full path if entry is nullptr
  Entry* entry = nullptr;
  std::vector<EntryTreeNode> children;  // children, sorted by name
  int selected = 1;
};
}  // namespace

static std::map<std::string, std::unique_ptr<InputFile>, std::less<>>
    gInputFiles;
static wpi::util::mutex gEntriesMutex;
static std::map<std::string, std::unique_ptr<Entry>, std::less<>> gEntries;
static std::vector<EntryTreeNode> gEntryTree;
std::atomic_int gExportCount{0};

// must be called with gEntriesMutex held
static void RebuildEntryTree() {
  gEntryTree.clear();
  wpi::util::SmallVector<std::string_view, 16> parts;
  for (auto& kv : gEntries) {
    parts.clear();
    // split on first : if one is present
    auto [prefix, mainpart] = wpi::util::split(kv.first, ':');
    if (mainpart.empty() || wpi::util::contains(prefix, '/')) {
      mainpart = kv.first;
    } else {
      parts.emplace_back(prefix);
    }
    wpi::util::split(mainpart, '/', -1, false,
                     [&](auto part) { parts.emplace_back(part); });

    // ignore a raw "/" key
    if (parts.empty()) {
      continue;
    }

    // get to leaf
    auto nodes = &gEntryTree;
    for (auto part :
         wpi::util::drop_back(std::span{parts.begin(), parts.end()})) {
      auto it =
          std::find_if(nodes->begin(), nodes->end(),
                       [&](const auto& node) { return node.name == part; });
      if (it == nodes->end()) {
        nodes->emplace_back(part);
        // path is from the beginning of the string to the end of the current
        // part; this works because part is a reference to the internals of
        // kv.first
        nodes->back().path.assign(kv.first.data(),
                                  part.data() + part.size() - kv.first.data());
        it = nodes->end() - 1;
      }
      nodes = &it->children;
    }

    auto it = std::find_if(nodes->begin(), nodes->end(), [&](const auto& node) {
      return node.name == parts.back();
    });
    if (it == nodes->end()) {
      nodes->emplace_back(parts.back());
      // no need to set path, as it's identical to kv.first
      it = nodes->end() - 1;
    }
    it->entry = kv.second.get();
  }
}

InputFile::InputFile(std::unique_ptr<wpi::log::DataLogReaderThread> datalog_)
    : filename{datalog_->GetBufferIdentifier()},
      stem{fs::path{filename}.stem().string()},
      datalog{std::move(datalog_)} {
  datalog->sigEntryAdded.connect([this](const wpi::log::StartRecordData& srd) {
    std::scoped_lock lock{gEntriesMutex};
    auto it = gEntries.find(srd.name);
    if (it == gEntries.end()) {
      it = gEntries.emplace(srd.name, std::make_unique<Entry>(srd)).first;
      RebuildEntryTree();
    } else {
      if (it->second->type != srd.type) {
        it->second->typeConflict = true;
      }
      if (it->second->metadata != srd.metadata) {
        it->second->metadataConflict = true;
      }
    }
    it->second->inputFiles.emplace(this);
  });
}

InputFile::~InputFile() {
  if (gShutdown || !datalog) {
    return;
  }
  std::scoped_lock lock{gEntriesMutex};
  bool changed = false;
  for (auto it = gEntries.begin(); it != gEntries.end();) {
    it->second->inputFiles.erase(this);
    if (it->second->inputFiles.empty()) {
      it = gEntries.erase(it);
      changed = true;
    } else {
      ++it;
    }
  }
  if (changed) {
    RebuildEntryTree();
  }
}

static std::unique_ptr<InputFile> LoadDataLog(std::string_view filename) {
  auto fileBuffer = wpi::util::MemoryBuffer::GetFile(filename);
  if (!fileBuffer) {
    return std::make_unique<InputFile>(
        filename,
        std::format("Could not open file: {}", fileBuffer.error().message()));
  }

  wpi::log::DataLogReader reader{std::move(*fileBuffer)};
  if (!reader.IsValid()) {
    return std::make_unique<InputFile>(filename, "Not a valid datalog file");
  }

  return std::make_unique<InputFile>(
      std::make_unique<wpi::log::DataLogReaderThread>(std::move(reader)));
}

void DisplayInputFiles() {
  static std::unique_ptr<pfd::open_file> dataFileSelector;

  SetNextWindowPos(ImVec2{0, 20}, ImGuiCond_FirstUseEver);
  SetNextWindowSize(ImVec2{375, 230}, ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Input Files")) {
    if (ImGui::Button("Open File(s)...")) {
      dataFileSelector = std::make_unique<pfd::open_file>(
          "Select Data Log", "",
          std::vector<std::string>{"DataLog Files", "*.wpilog"},
          pfd::opt::multiselect);
    }
    ImGui::BeginTable(
        "Input Files", 3,
        ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp);
    ImGui::TableSetupColumn("File");
    ImGui::TableSetupColumn("Status");
    ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed |
                                     ImGuiTableColumnFlags_NoHeaderLabel |
                                     ImGuiTableColumnFlags_NoHeaderWidth);
    ImGui::TableHeadersRow();
    for (auto it = gInputFiles.begin(); it != gInputFiles.end();) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      if (it->second->highlight) {
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                               IM_COL32(0, 64, 0, 255));
        it->second->highlight = false;
      }
      ImGui::TextUnformatted(it->first.c_str());
      if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", it->second->filename.c_str());
      }

      ImGui::TableNextColumn();
      if (it->second->datalog) {
        ImGui::Text("%u records, %u entries%s",
                    it->second->datalog->GetNumRecords(),
                    it->second->datalog->GetNumEntries(),
                    it->second->datalog->IsDone() ? "" : " (working)");
      } else {
        ImGui::TextUnformatted(it->second->status.c_str());
      }

      ImGui::TableNextColumn();
      ImGui::PushID(it->first.c_str());
      if (ImGui::SmallButton("X")) {
        it = gInputFiles.erase(it);
        gExportCount = 0;
      } else {
        ++it;
      }
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
  ImGui::End();

  // Load data file(s)
  if (dataFileSelector && dataFileSelector->ready(0)) {
    auto result = dataFileSelector->result();
    for (auto&& filename : result) {
      // don't allow duplicates
      std::string stem = fs::path{filename}.stem().string();
      auto it = gInputFiles.find(stem);
      if (it == gInputFiles.end()) {
        gInputFiles.emplace(std::move(stem), LoadDataLog(filename));
        gExportCount = 0;
      }
    }
    dataFileSelector.reset();
  }
}

static bool EmitEntry(const std::string& name, Entry& entry) {
  ImGui::TableNextColumn();
  bool rv = ImGui::Checkbox(name.c_str(), &entry.selected);
  if (ImGui::IsItemHovered() && gInputFiles.size() > 1) {
    for (auto inputFile : entry.inputFiles) {
      inputFile->highlight = true;
    }
  }

  ImGui::TableNextColumn();
  if (entry.typeConflict) {
    ImGui::TextUnformatted("(Inconsistent)");
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      for (auto inputFile : entry.inputFiles) {
        if (auto info = inputFile->datalog->GetEntry(entry.name)) {
          ImGui::Text("%s: %s", inputFile->stem.c_str(),
                      std::string{info->type}.c_str());
        }
      }
      ImGui::EndTooltip();
    }
  } else {
    ImGui::TextUnformatted(entry.type.c_str());
  }

  ImGui::TableNextColumn();
  if (entry.metadataConflict) {
    ImGui::TextUnformatted("(Inconsistent)");
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      for (auto inputFile : entry.inputFiles) {
        if (auto info = inputFile->datalog->GetEntry(entry.name)) {
          ImGui::Text("%s: %s", inputFile->stem.c_str(),
                      std::string{info->metadata}.c_str());
        }
      }
      ImGui::EndTooltip();
    }
  } else {
    ImGui::TextUnformatted(entry.metadata.c_str());
  }
  return rv;
}

static bool EmitEntryTree(std::vector<EntryTreeNode>& tree) {
  bool rv = false;
  for (auto&& node : tree) {
    if (node.entry) {
      if (EmitEntry(node.name, *node.entry)) {
        rv = true;
      }
    }

    if (!node.children.empty()) {
      ImGui::TableNextColumn();
      auto label = std::format("##check_{}", node.name);
      if (node.selected == -1) {
        ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);
        bool b = false;
        if (ImGui::Checkbox(label.c_str(), &b)) {
          node.selected = 3;  // 3 = enable group
          rv = true;
        }
        ImGui::PopItemFlag();
      } else {
        bool b = node.selected == 1 || node.selected == 3;
        if (ImGui::Checkbox(label.c_str(), &b)) {
          node.selected = b ? 3 : 2;  // 2 = disable group
          rv = true;
        }
      }
      ImGui::SameLine();
      bool open = ImGui::TreeNodeEx(node.name.c_str(),
                                    ImGuiTreeNodeFlags_SpanFullWidth);
      ImGui::TableNextColumn();
      ImGui::TableNextColumn();
      if (open) {
        if (EmitEntryTree(node.children)) {
          rv = true;
        }
        ImGui::TreePop();
      }
    }
  }
  return rv;
}

static void RefreshTreeCheckboxes(std::vector<EntryTreeNode>& tree,
                                  int* selected) {
  bool first = true;
  for (auto&& node : tree) {
    if (node.entry) {
      if (first && *selected == -1) {
        *selected = node.entry->selected ? 1 : 0;
      }
      if ((*selected == 0 && node.entry->selected) ||
          (*selected == 1 && !node.entry->selected)) {
        *selected = -1;             // inconsistent
      } else if (*selected == 2) {  // disable group
        node.entry->selected = false;
      } else if (*selected == 3) {  // enable group
        node.entry->selected = true;
      }
    }

    if (!node.children.empty()) {
      if (*selected == 2) {  // disable group
        node.selected = 2;
      } else if (*selected == 3) {  // enable group
        node.selected = 3;
      }
      RefreshTreeCheckboxes(node.children, &node.selected);
      if (node.selected == 2) {
        node.selected = 0;
      } else if (node.selected == 3) {
        node.selected = 1;
      }
      if (first && *selected == -1) {
        *selected = node.selected;
      } else if (node.selected == -1 ||
                 (*selected == 0 && node.selected == 1) ||
                 (*selected == 1 && node.selected == 0)) {
        *selected = -1;  // inconsistent
      }
    }

    first = false;
  }
}

void DisplayEntries() {
  SetNextWindowPos(ImVec2{380, 20}, ImGuiCond_FirstUseEver);
  SetNextWindowSize(ImVec2{540, 365}, ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Entries")) {
    static bool treeView = true;
    if (ImGui::BeginPopupContextItem()) {
      ImGui::MenuItem("Tree View", "", &treeView);
      ImGui::EndPopup();
    }
    std::scoped_lock lock{gEntriesMutex};
    ImGui::BeginTable(
        "Entries", 3,
        ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp);
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Type");
    ImGui::TableSetupColumn("Metadata");
    ImGui::TableHeadersRow();
    if (treeView) {
      if (EmitEntryTree(gEntryTree)) {
        int selected = -1;
        RefreshTreeCheckboxes(gEntryTree, &selected);
      }
    } else {
      for (auto&& kv : gEntries) {
        EmitEntry(kv.first, *kv.second);
      }
    }
    ImGui::EndTable();
  }
  ImGui::End();
}

static wpi::util::mutex gExportMutex;
static std::vector<std::string> gExportErrors;

static void ExportCsv(std::string_view outputFolder, int style,
                      int64_t timestampFuzziness) {
  std::set<std::string, std::less<>> selected;
  {
    std::scoped_lock lock{gEntriesMutex};
    for (auto&& [name, entry] : gEntries) {
      if (entry->selected) {
        selected.emplace(name);
      }
    }
  }
  fs::path outPath{outputFolder};
  for (auto&& f : gInputFiles) {
    if (f.second->datalog) {
      std::error_code ec;
      auto of = fs::OpenFileForWrite(
          outPath / fs::path{f.first}.replace_extension("csv"), ec,
          fs::CD_CreateNew, fs::OF_Text);
      if (ec) {
        std::scoped_lock lock{gExportMutex};
        gExportErrors.emplace_back(
            std::format("{}: {}", f.first, ec.message()));
        ++gExportCount;
        continue;
      }
      wpi::util::raw_fd_ostream os{fs::FileToFd(of, ec, fs::OF_Text), true};
      dlt::ExportCsv(f.second->datalog->GetReader(), os,
                     style == 0 ? dlt::CsvStyle::LIST : dlt::CsvStyle::TABLE,
                     timestampFuzziness, [&](std::string_view name) {
                       return selected.contains(name);
                     });
    }
    ++gExportCount;
  }
}

void DisplayOutput(wpi::glass::Storage& storage) {
  static std::string& outputFolder = storage.GetString("outputFolder");
  static double& timestampFuzzinessMs =
      storage.GetDouble("timestampFuzzinessMs");
  static std::unique_ptr<pfd::select_folder> outputFolderSelector;

  SetNextWindowPos(ImVec2{380, 390}, ImGuiCond_FirstUseEver);
  SetNextWindowSize(ImVec2{540, 120}, ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Output")) {
    if (ImGui::Button("Select Output Folder...")) {
      outputFolderSelector =
          std::make_unique<pfd::select_folder>("Select Output Folder");
    }
    ImGui::TextUnformatted(outputFolder.c_str());

    static const char* const options[] = {"List", "Table"};
    static int style = 0;
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
    ImGui::Combo("Style", &style, options,
                 sizeof(options) / sizeof(const char*));

    if (style == 1) {
      ImGui::SameLine();
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
      ImGui::InputDouble("Timestamp fuzziness (ms)", &timestampFuzzinessMs, 0.0,
                         0.0, "%.6f");
      ImGui::SetItemTooltip(
          "Group consecutive changes whose timestamps span at most this "
          "duration.\n"
          "Each row uses the earliest timestamp and the last value for each "
          "field.\n"
          "Zero merges only identical timestamps.");
    }
    // Keep conversion to integer nanoseconds in range, including saved
    // settings.
    constexpr double MAX_TIMESTAMP_FUZZINESS_MS =
        std::numeric_limits<int64_t>::max() / 1'000'000;
    if (!std::isfinite(timestampFuzzinessMs)) {
      timestampFuzzinessMs = 0;
    }
    timestampFuzzinessMs =
        std::clamp(timestampFuzzinessMs, 0.0, MAX_TIMESTAMP_FUZZINESS_MS);

    static std::future<void> exporter;
    if (!gInputFiles.empty() && !outputFolder.empty() &&
        ImGui::Button("Export CSV") &&
        (gExportCount == 0 ||
         gExportCount == static_cast<int>(gInputFiles.size()))) {
      gExportCount = 0;
      gExportErrors.clear();
      exporter = std::async(
          std::launch::async, ExportCsv, outputFolder, style,
          static_cast<int64_t>(std::round(timestampFuzzinessMs * 1'000'000.0)));
    }
    if (exporter.valid()) {
      ImGui::SameLine();
      ImGui::Text("Exported %d/%d", gExportCount.load(),
                  static_cast<int>(gInputFiles.size()));
    }
    {
      std::scoped_lock lock{gExportMutex};
      for (auto&& err : gExportErrors) {
        ImGui::TextUnformatted(err.c_str());
      }
    }
  }
  ImGui::End();

  if (outputFolderSelector && outputFolderSelector->ready(0)) {
    outputFolder = outputFolderSelector->result();
    outputFolderSelector.reset();
  }
}

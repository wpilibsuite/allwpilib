// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Exporter.h"

#include <atomic>
#include <ctime>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <glass/Storage.h>
#include <glass/support/DataLogReaderThread.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <portable-file-dialogs.h>
#include <wpi/DenseMap.h>
#include <wpi/MemoryBuffer.h>
#include <wpi/SmallVector.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/fmt/raw_ostream.h>
#include <wpi/fs.h>
#include <wpi/mutex.h>
#include <wpi/print.h>
#include <wpi/raw_ostream.h>

#include "App.h"

namespace {
struct InputFile {
  explicit InputFile(std::unique_ptr<glass::DataLogReaderThread> datalog);

  InputFile(std::string_view filename, std::string_view status)
      : filename{filename},
        stem{fs::path{filename}.stem().string()},
        status{status} {}

  ~InputFile();

  std::string filename;
  std::string stem;
  std::unique_ptr<glass::DataLogReaderThread> datalog;
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

  // used only during export
  int column = -1;
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
static wpi::mutex gEntriesMutex;
static std::map<std::string, std::unique_ptr<Entry>, std::less<>> gEntries;
static std::vector<EntryTreeNode> gEntryTree;
std::atomic_int gExportCount{0};

// must be called with gEntriesMutex held
static void RebuildEntryTree() {
  gEntryTree.clear();
  wpi::SmallVector<std::string_view, 16> parts;
  for (auto& kv : gEntries) {
    parts.clear();
    // split on first : if one is present
    auto [prefix, mainpart] = wpi::split(kv.first, ':');
    if (mainpart.empty() || wpi::contains(prefix, '/')) {
      mainpart = kv.first;
    } else {
      parts.emplace_back(prefix);
    }
    wpi::split(mainpart, parts, '/', -1, false);

    // ignore a raw "/" key
    if (parts.empty()) {
      continue;
    }

    // get to leaf
    auto nodes = &gEntryTree;
    for (auto part : wpi::drop_back(std::span{parts.begin(), parts.end()})) {
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

InputFile::InputFile(std::unique_ptr<glass::DataLogReaderThread> datalog_)
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
  auto fileBuffer = wpi::MemoryBuffer::GetFile(filename);
  if (!fileBuffer) {
    return std::make_unique<InputFile>(
        filename,
        fmt::format("Could not open file: {}", fileBuffer.error().message()));
  }

  wpi::log::DataLogReader reader{std::move(*fileBuffer)};
  if (!reader.IsValid()) {
    return std::make_unique<InputFile>(filename, "Not a valid datalog file");
  }

  return std::make_unique<InputFile>(
      std::make_unique<glass::DataLogReaderThread>(std::move(reader)));
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
      auto label = fmt::format("##check_{}", node.name);
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

static wpi::mutex gExportMutex;
static std::vector<std::string> gExportErrors;

static void PrintEscapedCsvString(wpi::raw_ostream& os, std::string_view str) {
  auto s = str;
  while (!s.empty()) {
    std::string_view fragment;
    std::tie(fragment, s) = wpi::split(s, '"');
    os << fragment;
    if (!s.empty()) {
      os << '"' << '"';
    }
  }
  if (wpi::ends_with(str, '"')) {
    os << '"' << '"';
  }
}

static void ValueToCsv(wpi::raw_ostream& os, const Entry& entry,
                       const wpi::log::DataLogRecord& record) {
  // handle systemTime specially
  if (entry.name == "systemTime" && entry.type == "int64") {
    int64_t val;
    if (record.GetInteger(&val)) {
      std::time_t timeval = val / 1000000;
      wpi::print(os, "{:%Y-%m-%d %H:%M:%S}.{:06}", *std::localtime(&timeval),
                 val % 1000000);
      return;
    }
  } else if (entry.type == "double") {
    double val;
    if (record.GetDouble(&val)) {
      wpi::print(os, "{}", val);
      return;
    }
  } else if (entry.type == "int64" || entry.type == "int") {
    // support "int" for compatibility with old NT4 datalogs
    int64_t val;
    if (record.GetInteger(&val)) {
      wpi::print(os, "{}", val);
      return;
    }
  } else if (entry.type == "string" || entry.type == "json") {
    std::string_view val;
    record.GetString(&val);
    os << '"';
    PrintEscapedCsvString(os, val);
    os << '"';
    return;
  } else if (entry.type == "boolean") {
    bool val;
    if (record.GetBoolean(&val)) {
      wpi::print(os, "{}", val);
      return;
    }
  } else if (entry.type == "boolean[]") {
    std::vector<int> val;
    if (record.GetBooleanArray(&val)) {
      wpi::print(os, "{}", fmt::join(val, ";"));
      return;
    }
  } else if (entry.type == "double[]") {
    std::vector<double> val;
    if (record.GetDoubleArray(&val)) {
      wpi::print(os, "{}", fmt::join(val, ";"));
      return;
    }
  } else if (entry.type == "float[]") {
    std::vector<float> val;
    if (record.GetFloatArray(&val)) {
      wpi::print(os, "{}", fmt::join(val, ";"));
      return;
    }
  } else if (entry.type == "int64[]") {
    std::vector<int64_t> val;
    if (record.GetIntegerArray(&val)) {
      wpi::print(os, "{}", fmt::join(val, ";"));
      return;
    }
  } else if (entry.type == "string[]") {
    std::vector<std::string_view> val;
    if (record.GetStringArray(&val)) {
      os << '"';
      bool first = true;
      for (auto&& v : val) {
        if (!first) {
          os << ';';
        }
        first = false;
        PrintEscapedCsvString(os, v);
      }
      os << '"';
      return;
    }
  }
  wpi::print(os, "<invalid>");
}

static void ExportCsvFile(InputFile& f, wpi::raw_ostream& os, int style) {
  // header
  if (style == 0) {
    os << "Timestamp,Name,Value\n";
  } else if (style == 1) {
    // scan for exported fields for this file to print header and assign columns
    os << "Timestamp";
    int columnNum = 0;
    for (auto&& entry : gEntries) {
      if (entry.second->selected &&
          entry.second->inputFiles.find(&f) != entry.second->inputFiles.end()) {
        os << ',' << '"';
        PrintEscapedCsvString(os, entry.first);
        os << '"';
        entry.second->column = columnNum++;
      } else {
        entry.second->column = -1;
      }
    }
    os << '\n';
  }

  wpi::DenseMap<int, Entry*> nameMap;
  for (auto&& record : f.datalog->GetReader()) {
    if (record.IsStart()) {
      wpi::log::StartRecordData data;
      if (record.GetStartData(&data)) {
        auto it = gEntries.find(data.name);
        if (it != gEntries.end() && it->second->selected) {
          nameMap[data.entry] = it->second.get();
        }
      }
    } else if (record.IsFinish()) {
      int entry;
      if (record.GetFinishEntry(&entry)) {
        nameMap.erase(entry);
      }
    } else if (!record.IsControl()) {
      auto entryIt = nameMap.find(record.GetEntry());
      if (entryIt == nameMap.end()) {
        continue;
      }
      Entry* entry = entryIt->second;

      if (style == 0) {
        wpi::print(os, "{},\"", record.GetTimestamp() / 1000000.0);
        PrintEscapedCsvString(os, entry->name);
        os << '"' << ',';
        ValueToCsv(os, *entry, record);
        os << '\n';
      } else if (style == 1 && entry->column != -1) {
        wpi::print(os, "{},", record.GetTimestamp() / 1000000.0);
        for (int i = 0; i < entry->column; ++i) {
          os << ',';
        }
        ValueToCsv(os, *entry, record);
        os << '\n';
      }
    }
  }
}

static void ExportCsv(std::string_view outputFolder, int style) {
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
            fmt::format("{}: {}", f.first, ec.message()));
        ++gExportCount;
        continue;
      }
      wpi::raw_fd_ostream os{fs::FileToFd(of, ec, fs::OF_Text), true};
      ExportCsvFile(*f.second, os, style);
    }
    ++gExportCount;
  }
}

void DisplayOutput(glass::Storage& storage) {
  static std::string& outputFolder = storage.GetString("outputFolder");
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

    static std::future<void> exporter;
    if (!gInputFiles.empty() && !outputFolder.empty() &&
        ImGui::Button("Export CSV") &&
        (gExportCount == 0 ||
         gExportCount == static_cast<int>(gInputFiles.size()))) {
      gExportCount = 0;
      gExportErrors.clear();
      exporter = std::async(std::launch::async, ExportCsv, outputFolder, style);
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

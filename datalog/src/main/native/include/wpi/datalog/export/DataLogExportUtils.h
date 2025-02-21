#pragma once

#include <memory>
#include <string>
#include <string_view>
#include "wpi/datalog/DataLogReaderThread.h"
#include "wpi/fs.h"

namespace wpi::log::fileexport {
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
  uint64_t id;
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

std::unique_ptr<InputFile> LoadDataLog(std::string_view filename);
}  // namespace wpi::log
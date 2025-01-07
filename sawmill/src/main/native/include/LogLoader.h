// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/DataLogReader.h>
#include <wpi/Signal.h>

namespace glass {
class Storage;
}  // namespace glass

namespace wpi {
class DataLogReaderEntry;
class DataLogReaderThread;
class Logger;
}  // namespace wpi

namespace datalogcli {
/**
 * Helps with loading datalog files.
 */
class LogLoader {
 public:
  /**
   * Creates a log loader
   */
  explicit LogLoader();

  ~LogLoader();

  /**
   * Signal called when the current file is unloaded (invalidates any
   * LogEntry*).
   */
  wpi::sig::Signal<> unload;

  void Load(std::string_view log_path);

  std::vector<wpi::log::DataLogRecord> GetRecords(std::string_view field_name);

  std::vector<wpi::log::DataLogRecord> GetAllRecords();

 private:
  std::string m_filename;
  std::unique_ptr<wpi::DataLogReaderThread> m_reader;

  std::string m_error;

  std::string m_filter;

  wpi::log::StartRecordData* entryData;

  struct EntryTreeNode {
    explicit EntryTreeNode(std::string_view name) : name{name} {}
    std::string name;  // name of just this node
    std::string path;  // full path if entry is nullptr
    const wpi::DataLogReaderEntry* entry = nullptr;
    std::vector<EntryTreeNode> children;  // children, sorted by name
  };
  std::vector<EntryTreeNode> m_entryTree;
};
}  // namespace datalogcli

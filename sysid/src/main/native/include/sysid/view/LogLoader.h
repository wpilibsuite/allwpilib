// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/Signal.h>

namespace glass {
class DataLogReaderEntry;
class DataLogReaderThread;
class Storage;
}  // namespace glass

namespace pfd {
class open_file;
}  // namespace pfd

namespace wpi {
class Logger;
}  // namespace wpi

namespace sysid {
/**
 * Helps with loading datalog files.
 */
class LogLoader {
 public:
  /**
   * Creates a log loader widget
   *
   * @param logger The program logger
   */
  explicit LogLoader(glass::Storage& storage, wpi::Logger& logger);

  ~LogLoader();

  /**
   * Displays the log loader window.
   */
  void Display();

  /**
   * Signal called when the current file is unloaded (invalidates any
   * LogEntry*).
   */
  wpi::sig::Signal<> unload;

 private:
  // wpi::Logger& m_logger;

  std::string m_filename;
  std::unique_ptr<pfd::open_file> m_opener;
  std::unique_ptr<glass::DataLogReaderThread> m_reader;

  std::string m_error;

  std::string m_filter;

  struct EntryTreeNode {
    explicit EntryTreeNode(std::string_view name) : name{name} {}
    std::string name;  // name of just this node
    std::string path;  // full path if entry is nullptr
    const glass::DataLogReaderEntry* entry = nullptr;
    std::vector<EntryTreeNode> children;  // children, sorted by name
  };
  std::vector<EntryTreeNode> m_entryTree;

  void RebuildEntryTree();
  void DisplayEntryTree(const std::vector<EntryTreeNode>& nodes);
};
}  // namespace sysid

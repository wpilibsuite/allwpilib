// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <wpi/datalog/DataLogReader.h>
#include <wpi/Signal.h>
#include <wpi/fs.h>

#include "DataLogExport.h"

namespace glass {
class Storage;
}  // namespace glass

namespace wpi::log {
class DataLogReaderEntry;
class DataLogReaderThread;
class Logger;
}  // namespace wpi

namespace sawmill {
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

  void Load(fs::path logPath);

  std::vector<sawmill::DataLogRecord> GetAllRecords();

  std::map<int, sawmill::Entry, std::less<>> GetEntryMap();

 private:
  std::string m_filename;
  std::unique_ptr<wpi::log::DataLogReaderThread> m_reader;

  std::string m_error;

  std::string m_filter;

  wpi::log::StartRecordData* entryData;

  std::map<int, sawmill::Entry, std::less<>> dataMap;

  std::vector<sawmill::DataLogRecord> records;
};
}  // namespace sawmill

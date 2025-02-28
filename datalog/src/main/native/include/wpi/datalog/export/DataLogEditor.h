#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include "wpi/datalog/DataLogReader.h"
#include "wpi/datalog/DataLogReaderThread.h"
#include "wpi/datalog/export/DataLogExportUtils.h"

namespace wpi::log {
class DataLogEditor {
 public:
  explicit DataLogEditor(const DataLogReaderThread& reader);

  DataLogEditor ExtractEntries(const std::vector<fileexport::Entry>& entries);
  DataLogEditor TrimToTime(uint64_t startTime, uint64_t endTime);
  DataLogEditor RenameEntry(std::string_view currentName, std::string newName);
  void ApplyEdits();

 private:
  std::vector<DataLogRecord> records;
  uint64_t startTime;
  uint64_t endTime;
  std::set<uint64_t> allowedEntries;
  std::map<std::string, std::string> entryRenames;
};
}  // namespace wpi::log
#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/datalog/DataLogReaderThread.hpp"

namespace wpi::log {
  class DataLogEditor {
    public:
    explicit DataLogEditor(std::unique_ptr<DataLogReaderThread> datalog);

    DataLogEditor WithTimestamps(std::vector<std::pair<int64_t, int64_t>> timestamps) const;

    DataLogEditor WithEntryNames(std::vector<std::string> entryNames) const;

    DataLogEditor WithRenamedEntries(std::map<std::string, std::string> renames) const;

    std::vector<DataLogRecord> ExecuteEdits();

    private:
    struct DataLogEditInfo {
      // record list (input to the filter function)
      std::vector<DataLogRecord> records;
      // used to get the initial record list and to check what entry a record is from
      std::unique_ptr<DataLogReaderThread> readerThread;

      // edit stage enable flags
      bool filterTimestamps = false;
      bool filterEntryNames = false;
      bool renameEntries = false;

      // edit info
      std::vector<std::pair<int64_t, int64_t>> allowedTimestamps;
      std::vector<std::string> allowedEntryNames;
      std::map<std::string, std::string> entryRenames;
    };

    explicit DataLogEditor(std::shared_ptr<DataLogEditInfo> config) : state(std::move(config)) {}

    std::shared_ptr<DataLogEditInfo> state;
  };
}
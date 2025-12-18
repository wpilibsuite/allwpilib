#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "wpi/datalog/DataLogReader.hpp"

namespace wpi::log {
  class DataLogEditor {
    public:
    explicit DataLogEditor(std::vector<DataLogRecord> initialRecords);

    DataLogEditor WithTimestamps(std::vector<std::pair<int64_t, int64_t>> timestamps) const;

    DataLogEditor WithEntryNames(std::vector<std::string> entryNames) const;

    DataLogEditor WithRenamedEntries(std::map<std::string, std::string> renames) const;

    std::vector<DataLogRecord> ExecuteEdits();

    private:
    struct DataLogEditInfo {
      std::vector<DataLogRecord> records;

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
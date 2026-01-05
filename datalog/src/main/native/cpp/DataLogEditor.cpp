#include "wpi/datalog/DataLogEditor.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/datalog/DataLogReaderThread.hpp"

namespace wpi::log {

  DataLogEditor::DataLogEditor(std::unique_ptr<DataLogReaderThread> datalog) {
    state = std::make_shared<DataLogEditInfo>();
    state->readerThread = std::move(datalog);
  }

  DataLogEditor DataLogEditor::WithTimestamps(std::vector<std::pair<int64_t, int64_t>> timestamps) const {
    state->filterTimestamps = true;
    state->allowedTimestamps = timestamps;
    return DataLogEditor{state};
  }

  DataLogEditor DataLogEditor::WithEntryNames(std::vector<std::string> entryNames) const {
    state->filterEntryNames = true;
    state->allowedEntryNames = entryNames;
    return DataLogEditor{state};
  }

  DataLogEditor DataLogEditor::WithRenamedEntries(std::map<std::string, std::string> renames) const {
    state->renameEntries = true;
    state->entryRenames = std::move(renames);
    return DataLogEditor{state};
  }

  std::vector<DataLogRecord> DataLogEditor::ExecuteEdits() {
    // we need to do the renames before we drop entries
    const DataLogReaderEntry* entryData;
    for (const DataLogRecord& record : state->records) {
      entryData = state->readerThread->GetEntry(record.GetEntry());
      // exclude based on timestamp
      auto timestamp = record.GetTimestamp();
      // if we allow this timestamp or it is a start/finish record, add it
      if (std::ranges::any_of(state->allowedTimestamps, [timestamp](const std::pair<int64_t, int64_t>& pair) {
        return timestamp >= pair.first && timestamp <= pair.second;
      }) || record.IsControl()) {
        state->records.push_back(record);
      }


      if (record.IsStart() && state->entryRenames.contains(std::string{entryData->name})) {
        // its the start record of the one we want and its guaranteed to be in the filtered list so lets replace it with a copy that includes the correct name
        StartRecordData srd;
        record.GetStartData(&srd);
        srd.name = state->entryRenames[std::string{srd.name}];
        auto it = std::find_if(state->records.begin(), state->records.end(), [srd](DataLogRecord& targetRecord) {
          return targetRecord.GetEntry() == srd.entry;
        });
        ptrdiff_t idx = it - state->records.begin();
        state->records.at(idx) = DataLogRecord{entryData->entry, timestamp, reinterpret_cast<std::span<uint8_t>>(srd)};
      }
    }
    return state->records;
  }
}
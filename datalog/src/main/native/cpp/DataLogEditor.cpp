#include "wpi/datalog/DataLogEditor.hpp"
#include <memory>
#include <utility>
#include <vector>
#include "wpi/datalog/DataLogReader.hpp"

namespace wpi::log {

  DataLogEditor::DataLogEditor(std::vector<DataLogRecord> initialRecords) {
    state = std::make_shared<DataLogEditInfo>();
    state->records = std::move(initialRecords);
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
}
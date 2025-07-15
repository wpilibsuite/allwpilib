#include "wpi/datalog/export/DataLogEditor.h"
#include <cstdint>
#include <vector>
#include "wpi/datalog/DataLogReaderThread.h"
#include "wpi/datalog/export/DataLogExportUtils.h"

namespace wpi::log {
  DataLogEditor::DataLogEditor(const DataLogReaderThread& reader) {
    for (auto record : reader.GetReader()) {
      records.push_back(record);
    }
  }

  DataLogEditor DataLogEditor::ExtractEntries(const std::vector<fileexport::Entry>& entries) {
    for (const auto& entry : entries) {
      allowedEntries.emplace(entry.id);
    }
    return *this;
  }

  DataLogEditor DataLogEditor::TrimToTime(uint64_t start, uint64_t end) {
    endTime = end;
    startTime = start;
    return *this;
  }
}


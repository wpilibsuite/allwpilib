// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LogLoader.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <fmt/base.h>
#include <wpi/datalog/DataLogReader.h>
#include <wpi/datalog/DataLogReaderThread.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>

#include "DataLogExport.h"

using namespace sawmill;

LogLoader::LogLoader() {}

LogLoader::~LogLoader() = default;

void LogLoader::Load(fs::path logPath) {
  // Handle opening the file
  std::error_code ec;
  auto buf = wpi::MemoryBuffer::GetFile(logPath.string());
  if (ec) {
    m_error = fmt::format("Could not open file: {}", ec.message());
    return;
  }

  wpi::log::DataLogReader reader{*std::move(buf)};
  if (!reader.IsValid()) {
    m_error = "Not a valid datalog file";
    return;
  }
  unload();  // release the actual file, we have the data in the reader now
  m_reader = std::make_unique<wpi::log::DataLogReaderThread>(std::move(reader));

  // Handle Errors
  fmt::println("{}", m_error);

  if (!m_reader) {
    return;
  }

  // Summary info
  fmt::println("{}", fs::path{m_filename}.stem().string().c_str());
  fmt::println("%u records, %u entries%s", m_reader->GetNumRecords(),
               m_reader->GetNumEntries(),
               m_reader->IsDone() ? "" : " (working)");

  if (!m_reader->IsDone()) {
    return;
  }
}

/*std::vector<wpi::log::DataLogRecord> LogLoader::GetRecords(
    std::string_view field_name) {
  std::vector<wpi::log::DataLogRecord> record_list{};

  const wpi::DataLogReaderEntry* entry = m_reader->GetEntry(field_name);
  for (wpi::DataLogReaderRange range : entry->ranges) {
    wpi::log::DataLogReader::iterator rangeReader = range.begin();
    while (!rangeReader->IsFinish()) {
      record_list.push_back(*rangeReader);
    }
  }

  return record_list;
}*/

std::vector<sawmill::DataLogRecord> LogLoader::GetAllRecords() {
  if (records.size() == 0) {
    // get all records
    for (wpi::log::DataLogRecord record : m_reader->GetReader()) {
      if (record.IsStart()) {
        wpi::log::StartRecordData data;
        if (record.GetStartData(&data)) {
          // associate an entry id with a StartRecordData
          dataMap.emplace(data.entry, sawmill::Entry{data});
        }
      } else if (record.IsFinish()) {
        // remove the association
        int entryId;
        if (record.GetFinishEntry(&entryId)) {
          dataMap.erase(entryId);
        }
      }
      int entryId = record.GetEntry();
      if (dataMap.contains(entryId)) {
        if (auto it = dataMap.find(entryId); it != dataMap.end()) {
          records.emplace_back(it->second, record);
        }
      }
    }
  }

  return records;
}

std::map<int, sawmill::Entry, std::less<>> LogLoader::GetEntryMap() {
  return dataMap;
}

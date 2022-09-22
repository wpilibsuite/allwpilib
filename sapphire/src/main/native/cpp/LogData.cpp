//
// Created by bagatelle on 6/2/22.
//

#include "LogData.h"

#include <map>
#include <string>
#include <memory>
#include <system_error>
#include <utility>
#include <vector>
#include <filesystem>
#include "wpi/fmt/raw_ostream.h"

#include <wpi/MemoryBuffer.h>
#include <wpi/DataLogReader.h>

bool LogData::LoadWPILog(std::string filename) {
  std::error_code ec;
  auto buf = wpi::MemoryBuffer::GetFile(filename.c_str(), ec);
  std::string fn{filename};
  if (ec) {
    return false;
  }

  wpi::log::DataLogReader reader{std::move(buf)};
  if (!reader.IsValid()) {
    return false;
  }

  for (auto record : reader) {
    if(record.IsStart()) {
      wpi::log::StartRecordData data;
      if(record.GetStartData(&data)) {
        Entry entry;
        entry.start = data;
        m_entries.emplace(data.entry, entry);
      }
    } else if(record.IsFinish()) {
      int data;
      if(record.GetFinishEntry(&data)) {
        auto entryId = m_entries.find(data);
        if(entryId == m_entries.end()) {
          continue;
        }
        entryId->second.finishTimestamp = record.GetTimestamp();
      }
    } else if(!record.IsControl()) {
      auto entryId = m_entries.find(record.GetEntry());
      if(entryId == m_entries.end()) {
        continue;
      }
      Entry entry = entryId->second;
      
      int timestamp = record.GetTimestamp();
      if(timestamp > entry.finishTimestamp) {
        fmt::print("Entry Finished, Invalid Data Point\n");
        continue;
      }
      
      if(timestamp > m_maxTimestamp) {
        m_maxTimestamp = timestamp;
      }

      entry.datapoints.emplace(record.GetTimestamp(), record);
    }
  }
  
  m_hasLog = true;

  return true;
}
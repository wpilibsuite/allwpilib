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
  if (ec) {
    return false;
  }

  wpi::log::DataLogReader reader{std::move(buf)};
  if (!reader.IsValid()) {
    return false;
  }

  for (const auto& record : reader) {
    int entryId;
    if(record.IsStart()) {
      // If we find a new start record, create a new entry
      wpi::log::StartRecordData start;
      
      if(!record.GetStartData(&start)) { continue; }
      if(m_entries.find(start.entry) != m_entries.end()){ continue; } // This should probably be an error

      EntryData entry_data;
      entry_data.start = start;
      
      m_entries[start.entry] = entry_data;
    
    } else if(record.GetFinishEntry(&entryId)) {
      // If we find a finish entry,
      auto entryPair= m_entries.find(entryId);
      if(entryPair == m_entries.end()) { continue; }
    
      entryPair->second.finishTimestamp = record.GetTimestamp();
    } else if(!record.IsControl()) {
      auto entryPair = m_entries.find(record.GetEntry());
      if(entryPair == m_entries.end()) {
        continue;
      }
      EntryData &entry = entryPair->second;
      
      int timestamp = record.GetTimestamp();
      if(timestamp > entry.finishTimestamp) {
        fmt::print("Entry Finished, Invalid Data Point\n");
        continue;
      }
      
      if(timestamp > m_maxTimestamp) {
        m_maxTimestamp = timestamp;
      }

      entry.datapoints[record.GetTimestamp()] = record;
    }
  }
  
  m_hasLog = true;

  return true;
}

void LogData::AddEntryNode(EntryData& node, std::string path){
  std::shared_ptr<EntryData> data_ptr = std::shared_ptr<EntryData>(&node);
}

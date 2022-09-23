//
// Created by bagatelle on 6/2/22.
//

#include "DataLog.h"

#include <map>
#include <string>
#include <memory>
#include <system_error>
#include <utility>
#include <vector>
#include <filesystem>
#include "wpi/fmt/raw_ostream.h"
#include "fmt/format.h"

#include <wpi/MemoryBuffer.h>
#include <wpi/DataLogReader.h>
#include <imgui.h>

using namespace sapphire;

static std::vector<EntryView> entries;
static float maxTimestamp= 100;

wpi::log::DataLogRecord EntryData::GetRecordAt(int timestamp){
  wpi::log::DataLogRecord record;
  for(auto& entry : datapoints){
    if(entry.first <= timestamp){
      record = entry.second;
    }
  }
  return record;
}


bool DataLogModel::LoadWPILog(std::string filename) {
  std::error_code ec;
  auto buf = wpi::MemoryBuffer::GetFile(filename.c_str(), ec);
  if (ec) {
    return false;
  }
  
  reader = std::make_shared<wpi::log::DataLogReader>(std::move(buf));
  if (!reader->IsValid()) {
    return false;
  }

  for (const auto& record : *reader) {
    int entryId;
    if(record.IsStart()) {
      // If we find a new start record, create a new entry
      wpi::log::StartRecordData start;
      
      if(!record.GetStartData(&start)) { continue; }
      if(m_entries.find(start.entry) != m_entries.end()){ continue; } // This should probably be an error

      EntryData entry_data{start};
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

      entry.datapoints[record.GetTimestamp()] = std::move(record);
    }
  }
  
  m_hasLog = true;

  return true;
}

void DataLogModel::AddEntryNode(EntryData& node, std::string path){
  std::shared_ptr<EntryData> data_ptr = std::shared_ptr<EntryData>(&node);
}


void DataLogView::Display() {
    
    ImGui::Text("Manage Entry Time:");
    ImGui::SameLine();
    ImGui::SliderFloat("Timestamp", &timestamp ,0, maxTimestamp);
    bool update = ImGui::Button("Update");
    if(update){
        fmt::print("update!!");
    }

    if(ImGui::CollapsingHeader("TestHeader")){
        for(auto& entry : entries){
            entry.Display(true, timestamp);
        }
    }

    ImGui::Text("Entry Information: #Entries: %d, Max Timestamp: %d", entries.size(), maxTimestamp);
}

void DataLogView::DisplayDataLog(DataLogModel& logData){
    entries.clear();

    if(!logData.Exists()){
        return;
    }
    
    maxTimestamp = logData.GetMaxTimestamp() / 1000000.0;
    for(auto& entry : logData.m_entries){
       
       EntryView view{&entry.second};
       entries.emplace_back(view);
    }

}

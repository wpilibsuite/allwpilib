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
static float maxTimestamp = 100;
static bool needsUpdate = false;

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
  this->m_entries.clear();
  for (const auto& record : *reader) {
    int entryId;
    if(record.IsStart()) {
      // If we find a new start record, create a new entry
      wpi::log::StartRecordData start;
      
      if(!record.GetStartData(&start)) { continue; }
      if(m_entries.find(start.entry) != m_entries.end()){ continue; } // This should probably be an error

      EntryData entry_data{start};
      m_entries[start.entry] = entry_data;
      AddEntryNode(&m_entries[start.entry], m_entries[start.entry].name);
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


EntryNode* find(std::vector<EntryNode>& list, std::string name){
  for(auto& child : list){
    if(child.name == name){
      return &child;
    }
  }
  return nullptr;
}


void DataLogModel::AddEntryNode(EntryData* data, std::string path){
  std::vector<std::string> pathVec;
  int last = 0;
  for(int i = 0; i < path.length(); i++){
    if(path[i] == '/'){
      pathVec.emplace_back(path.substr(last, i-last));
      last = i;
    }
  }
  pathVec.emplace_back(path.substr(last, path.length()-last));
  if(pathVec.size() > 0){
    auto nextNode = find(m_tree, pathVec[0]);
    if(nextNode == nullptr){
      nextNode = &m_tree.emplace_back(EntryNode{pathVec[0]});
    }
    nextNode->AddEntry(data, std::vector<std::string>(pathVec.begin()+1, pathVec.end()));
  }
}

void EntryNode::AddEntry(EntryData *entry,std::vector<std::string> path){
  if(path.size() > 0){
    auto nextNode = find(children, path[0]);
    if(nextNode == nullptr){
      nextNode = &children.emplace_back(EntryNode{path[0]});
      return;
    }
    nextNode->AddEntry(entry, std::vector<std::string>(path.begin()+1, path.end()));
  } else {
    // We are the final destination for the data
    this->entry = entry;
  }

}


std::string EntryNode::TreeToString(int depth){
  std::string ret = fmt::format("T:{} \n", this->name);
  for(auto &child : children){
    for(int i = 0; i < depth; i++){
      ret += fmt::format("-");
    }
    ret += child.TreeToString(depth+1) + "\n";
  }
  return ret;
}

std::string sapphire::GetFormattedEntryValue(EntryData& data, float timestamp){
    int expandedts = static_cast<int>(timestamp*1000000);
    auto record = data.GetRecordAt(expandedts);
    if(record.GetEntry() == -1){ return "";}

    if (data.type == "double") {
        double val;
        if (record.GetDouble(&val)) {
            return fmt::format("  {}\n", val);
        }
    } else if (data.type == "int64") {
        int64_t val;
        if (record.GetInteger(&val)) {
            return fmt::format("  {}\n", val);
        }
    } else if (data.type == "string" ||
            data.type == "json") {
        std::string_view val;
        record.GetString(&val);
        return fmt::format("  '{}'\n", val);
    } else if (data.type == "boolean") {
        bool val;
        if (record.GetBoolean(&val)) {
            return fmt::format("  {}\n", val);
        }
    } else if (data.type == "boolean[]") {
        std::vector<int> val;
        if (record.GetBooleanArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        }
    } else if (data.type == "double[]") {
        std::vector<double> val;
        if (record.GetDoubleArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        }
    } else if (data.type == "float[]") {
        std::vector<float> val;
        if (record.GetFloatArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        }
    } else if (data.type == "int64[]") {
        std::vector<int64_t> val;
        if (record.GetIntegerArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        }
    } else if (data.type == "string[]") {
        std::vector<std::string_view> val;
        if (record.GetStringArray(&val)) {
            return fmt::format("  {}\n", fmt::join(val, ", "));
        }
    }
    return "  invalid";
}

void EmitEntry(EntryData *data, std::string name, float timestamp){
  ImGui::Text(name.c_str());
  ImGui::NextColumn();
  std::string value = GetFormattedEntryValue(*data, timestamp);
  ImGui::Text(value.c_str());
  ImGui::NextColumn();
}

void EmitTree(const std::vector<EntryNode>& tree, float timestamp) {
  for(auto &&node : tree){
    if(node.entry){
      EmitEntry(node.entry, node.name, timestamp);
    }
    if(!node.children.empty()){
      bool open = ImGui::TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
      ImGui::NextColumn();
      ImGui::NextColumn();
      ImGui::Separator();
      if(open){
        EmitTree(tree, timestamp);
        ImGui::TreePop();
      }
    }
  }
}


void DataLogView::Display() {
    
    ImGui::Text("Manage Entry Time:");
    ImGui::SameLine();
    ImGui::SliderFloat("Timestamp", &timestamp ,0, maxTimestamp);
    bool update = ImGui::Button("Update");
    if(update){
        fmt::print("update!!");
    }

    if(ImGui::CollapsingHeader("Values")){
        for(auto& entry : entries){
            entry.Display(true, timestamp);
        }
    }
    if (ImGui::CollapsingHeader("Tree")) {
      EmitTree(logData.model.GetTreeRoot(), timestamp);
    }

    if(logData.needsUpdate){
        Refresh();
        logData.needsUpdate = false;
    }
}

void DataLogView::Refresh(){
    entries.clear();

    if(!logData.model.Exists()){
        return;
    }
    
    maxTimestamp = logData.model.GetMaxTimestamp() / 1000000.0;
    for(auto& entry : logData.model.m_entries){
       EntryView view{&entry.second};
       entries.emplace_back(view);
    }
}

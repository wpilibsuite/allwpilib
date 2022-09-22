//
// Created by bagatelle on 6/2/22.
//

#ifndef ALLWPILIB_LOGDATA_H
#define ALLWPILIB_LOGDATA_H

#include <string>
#include <filesystem>
#include <map>
#include <utility>
#include <vector>
#include <unordered_map>

#include "wpi/DataLogReader.h"
#include "wpi/DenseMap.h"

struct EntryData {
  wpi::log::StartRecordData start;
  wpi::DenseMap<int, wpi::log::DataLogRecord> datapoints;
  int finishTimestamp = INT_MAX;
};

struct EntryNode {
  explicit EntryNode(std::string_view name, std::shared_ptr<EntryData> entry) : name{name}, entry{entry} {}

  std::shared_ptr<EntryData> GetEntry(std::string path);

  void AddEntry(std::shared_ptr<EntryData> entry,std::string path);

  std::string name;
  std::shared_ptr<EntryData> entry = nullptr;
  std::unordered_map<std::string, std::shared_ptr<EntryNode> > children;
};

class LogData {
  public:

    bool LoadWPILog(std::string filename);
    
    int GetMaxTimestamp() const { return m_maxTimestamp; };
    bool Exists() const { return m_hasLog; };
    void AddEntryNode(EntryData& node, std::string path);

    wpi::DenseMap<int, EntryData> m_entries;
    std::unordered_map<std::string, EntryNode> m_tree;

  private:
    int m_maxTimestamp;
    bool m_hasLog;

};

#endif  // ALLWPILIB_LOGDATA_H
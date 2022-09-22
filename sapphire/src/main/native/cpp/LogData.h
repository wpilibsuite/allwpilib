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

#include "wpi/DataLogReader.h"

class LogData {
  public:
    struct Entry {
      wpi::log::StartRecordData start;
      std::map<int, wpi::log::DataLogRecord> datapoints;
      int finishTimestamp = 1999999999;
    };

    struct TreeNode {
      explicit TreeNode(std::string_view name) : name{name} {}

      std::string name;

      std::string path;

      Entry* entry = nullptr;

      std::vector<TreeNode> children;
    };

    bool LoadWPILog(std::string filename);
    
    int GetMaxTimestamp() const { return m_maxTimestamp; };
    bool Exists() const { return m_hasLog; };
    
    std::map<int, Entry> m_entries;
  
  private:
    int m_maxTimestamp;
    bool m_hasLog;
};

#endif  // ALLWPILIB_LOGDATA_H

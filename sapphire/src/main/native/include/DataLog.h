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
#include "glass/Model.h"
#include "glass/View.h"
#include "EntryManager.h"
namespace sapphire{

std::string GetFormattedEntryValue(EntryData& data, float timestamp);

struct EntryData {
  EntryData() {}
  EntryData(wpi::log::StartRecordData start) : entry{start.entry}, 
                                               name{start.name}, 
                                               type{start.type}, 
                                               metadata{start.metadata} {};   
  wpi::log::DataLogRecord GetRecordAt(int timestamp);
  
  int entry;
  std::string name;
  std::string type;
  std::string metadata;

  std::map<int, wpi::log::DataLogRecord> datapoints;
  int finishTimestamp = INT_MAX;
};

struct EntryNode {
  explicit EntryNode(std::string_view name): name{name} {}

  std::shared_ptr<EntryData> GetEntry(std::vector<std::string> path);

  void AddEntry(EntryData *entry,std::vector<std::string> path);

  std::string name;
  EntryData *entry = nullptr;
  std::vector<EntryNode> children;
  std::string TreeToString(int depth = 1);

  
};



class DataLogModel : private glass::Model {
  public:
    DataLogModel():reader{nullptr}{};
    bool LoadWPILog(std::string filename);
    
    int  GetMaxTimestamp() const { return m_maxTimestamp; }
    void Update() override { }
    bool Exists() override { return m_hasLog; }
    int  GetSize() const {return m_entries.size(); }
    void AddEntryNode(EntryData* data, std::string path);
    const std::vector<EntryNode>& GetTreeRoot(){return m_tree;}

    wpi::DenseMap<int, EntryData> m_entries;
    std::vector<EntryNode> m_tree;

  private:
    std::shared_ptr<wpi::log::DataLogReader> reader ;
    int  m_maxTimestamp;
    bool m_hasLog;
};


struct DataLogReference {
    bool needsUpdate = false;
    DataLogModel model;
};

class DataLogView: public glass::View {
  public:
    DataLogView(DataLogReference& logData) : logData{logData} {}
    void Display() override;
    std::string name;
    DataLogReference& logData;
    void Refresh();
  private:
    float timestamp;
};


} // namespace sapphire
#endif  // ALLWPILIB_LOGDATA_H
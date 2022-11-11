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
namespace sapphire{

// Contains records of a single datalog entry
// Analogous to DataSource
struct EntryData {
  EntryData() {}
  EntryData(wpi::log::StartRecordData start) : entry{start.entry}, 
                                               name{start.name}, 
                                               type{start.type}, 
                                               metadata{start.metadata} {};   
  wpi::log::DataLogRecord GetRecordAt(int timestamp);
  wpi::log::DataLogRecord GetNextRecord(int timestamp);
  
  std::string GetName() const { return name; }
  void SetOffsetPtr(float *offset) {this->offset = offset;}
  float GetOffset() const {return *offset; }
  std::map<int, wpi::log::DataLogRecord>::iterator GetIterator(int timestamp);
  int entry;
  std::string name;
  std::string type;
  std::string metadata;
  float *offset = nullptr;
  std::map<int, wpi::log::DataLogRecord> datapoints;
  int finishTimestamp = INT_MAX;
};

std::string GetFormattedEntryValue(EntryData& data, int timestamp, wpi::log::DataLogRecord record);

// Contains information about the structure of the datalog
struct EntryNode {
  explicit EntryNode(std::string_view name): name{name} {}
  EntryData* GetEntry(std::vector<std::string> path);

  EntryNode* GetEntryNode(std::vector<std::string> path);
  void AddEntry(EntryData *entry,std::vector<std::string> path);

  std::string name;
  EntryData *entry = nullptr;
  std::vector<EntryNode> children;
  std::string TreeToString(int depth = 1);
};

struct DataLogFlags{
  bool ShowLastUpdate = false;
  bool ShowNextUpdate = false;
  bool IsLogActive = true;
};

// Model of the datalog. Used for the table view, as well as the object
// Used by other classes to parse datalogs
class DataLogModel : private glass::Model {
  public:
    DataLogModel():reader{nullptr}{};
    bool LoadWPILog(std::string filename);
    
    int  GetMaxTimestamp() const { return m_maxTimestamp; }
    void Update() override { }
    bool Exists() override { return m_hasLog && flags.IsLogActive; }
    int  GetSize() const {return m_entries.size(); }
    EntryNode* GetEntryNode(std::string path);
    void AddEntryNode(EntryData* data, std::string path);
    const std::vector<EntryNode>& GetTreeRoot(){return m_tree;}

    wpi::DenseMap<int, std::unique_ptr<EntryData> > m_entries;
    std::vector<EntryNode> m_tree;
    std::string filename = "";
    float offset = 0;
    DataLogFlags flags;
  private:
    std::shared_ptr<wpi::log::DataLogReader> reader ;
    int  m_maxTimestamp;
    bool m_hasLog;
};


class DataLogView: public glass::View {
  public:
    DataLogView(std::vector<std::unique_ptr<DataLogModel> >& logs) : logs{logs} {}
    void DisplayDataLog(DataLogModel*);
    void Display() override;
    int GetMaxTimestamp();
    float& GetTimestamp();
    std::string name;
    std::vector<std::unique_ptr<DataLogModel> >& logs;
};


} // namespace sapphire
#endif  // ALLWPILIB_LOGDATA_H
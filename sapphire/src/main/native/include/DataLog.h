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
  EntryData(wpi::log::StartRecordData start) : m_entry{start.entry}, 
                                               m_name{start.name}, 
                                               m_type{start.type}, 
                                               m_metadata{start.metadata} {};   
  wpi::log::DataLogRecord GetRecordAt(int timestamp);
  wpi::log::DataLogRecord GetNextRecord(int timestamp);
  
  std::string GetName() const { return m_name; }
  void SetOffsetPtr(float *offset) {this->offset = offset;}
  float GetOffset() const {return *offset; }
  std::map<int, wpi::log::DataLogRecord>::iterator GetIterator(int timestamp);
  int m_entry;
  std::string m_name;
  std::string m_type;
  std::string m_metadata;
  float *offset = nullptr;
  std::map<int, wpi::log::DataLogRecord> m_datapoints;
  int finishTimestamp = INT_MAX;
};

std::string GetFormattedEntryValue(EntryData& data, int timestamp, wpi::log::DataLogRecord record);

// Contains information about the structure of the datalog
struct EntryNode {
  explicit EntryNode(std::string_view name): m_name{name} {}
  EntryData* GetEntry(std::vector<std::string> path);

  EntryNode* GetEntryNode(std::vector<std::string> path);
  void AddEntry(EntryData *entry,std::vector<std::string> path);

  std::string m_name;
  EntryData* m_entry = nullptr;
  std::vector<EntryNode> m_children;
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
    DataLogModel():m_reader{nullptr}{};
    bool LoadWPILog(std::string filename);
    
    int  GetMaxTimestamp() const { return m_maxTimestamp; }
    void Update() override { }
    bool Exists() override { return m_hasLog && m_flags.IsLogActive; }
    int  GetSize() const {return m_entries.size(); }
    EntryNode* GetEntryNode(std::string path);
    void AddEntryNode(EntryData* data, std::string path);
    const std::vector<EntryNode>& GetTreeRoot(){return m_tree;}

    wpi::DenseMap<int, std::unique_ptr<EntryData> > m_entries;
    std::vector<EntryNode> m_tree;
    std::string m_path = "";
    std::string m_rawFilename = "";
    std::string m_filename = "";
    float m_offset = 0;
    DataLogFlags m_flags;
  private:
    std::shared_ptr<wpi::log::DataLogReader> m_reader;
    int  m_maxTimestamp;
    bool m_hasLog;
};


class DataLogView: public glass::View {
  public:
    DataLogView(std::vector<std::unique_ptr<DataLogModel> >& logs) : m_logs{logs} {}
    void DisplayDataLog(DataLogModel*);
    void Display() override;
    int GetMaxTimestamp();
    float& GetTimestamp();
    std::string m_name;
    std::vector<std::unique_ptr<DataLogModel> >& m_logs;
};


} // namespace sapphire
#endif  // ALLWPILIB_LOGDATA_H
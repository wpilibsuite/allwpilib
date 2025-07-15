#include <string_view>
#include <vector>
#include "wpi/Logger.h"
#include "wpi/datalog/DataLogBackgroundWriter.h"
#include "wpi/datalog/DataLogReader.h"
#include <map>
#include "wpi/datalog/export/DataLogExport.h"

using namespace wpi::log::fileexport;

void DataLogRecordExporter::ExportDataLog(
    std::string_view fullPath, std::vector<wpi::log::DataLogRecord> records,
    wpi::Logger& msgLogger) {
  fs::path path{fullPath};

  wpi::log::DataLogBackgroundWriter log{msgLogger, path.parent_path().string(),
                                        path.filename().string()};
  std::map<int, std::string_view> entryTypeMap;
  for (wpi::log::DataLogRecord record : records) {
    // start with control records
    if (record.IsControl()) {
      if (record.IsStart()) {
        wpi::log::StartRecordData srd;
        record.GetStartData(&srd);
        entryTypeMap[srd.entry] = srd.type;
        log.Start(srd.name, srd.type, srd.metadata, record.GetTimestamp());
      }

      if (record.IsFinish()) {
        int entry;
        if (record.GetFinishEntry(&entry)) {
          entryTypeMap.erase(entryTypeMap.find(entry));
          log.Finish(entry, record.GetTimestamp());
        }
      }

      if (record.IsSetMetadata()) {
        wpi::log::MetadataRecordData mrd;
        if (record.GetSetMetadataData(&mrd)) {
          log.SetMetadata(mrd.entry, mrd.metadata, record.GetTimestamp());
        }
      }
    }

    int entry = record.GetEntry();
    int64_t timestamp = record.GetTimestamp();

    if (entryTypeMap[entry] == "int64") {
      int64_t val;
      record.GetInteger(&val);
      log.AppendInteger(entry, val, timestamp);
    } else if (entryTypeMap[entry] == "double") {
      double val;
      record.GetDouble(&val);
      log.AppendDouble(entry, val, timestamp);
    } else if (entryTypeMap[entry] == "float") {
      float val;
      record.GetFloat(&val);
      log.AppendFloat(entry, val, timestamp);
    } else if (entryTypeMap[entry] == "boolean") {
      bool val;
      record.GetBoolean(&val);
      log.AppendBoolean(entry, val, timestamp);
    } else if (entryTypeMap[entry] == "string") {
      std::string_view val;
      record.GetString(&val);
      log.AppendString(entry, val, timestamp);
    } else if (entryTypeMap[entry] == "raw") {
      std::span<const uint8_t> data = record.GetRaw();
      log.AppendRaw(entry, data, timestamp);
    } else if (entryTypeMap[entry] == "int64[]") {
      std::vector<int64_t> val;
      record.GetIntegerArray(&val);
      log.AppendIntegerArray(entry, val, timestamp);
    } else if (entryTypeMap[entry] == "double[]") {
      std::vector<double> val;
      record.GetDoubleArray(&val);
      log.AppendDoubleArray(entry, val, timestamp);
    } else if (entryTypeMap[entry] == "float[]") {
      std::vector<float> val;
      record.GetFloatArray(&val);
      log.AppendFloatArray(entry, val, timestamp);
    } else if (entryTypeMap[entry] == "boolean[]") {
      std::vector<int> val;
      record.GetBooleanArray(&val);
      log.AppendBooleanArray(entry, val, timestamp);
    } else if (entryTypeMap[entry] == "string[]") {
      std::vector<std::string_view> val;
      record.GetStringArray(&val);
      log.AppendStringArray(entry, val, timestamp);
    }
  }
}

void DataLogRecordExporter::ExportCSV(
    std::string_view fullPath, std::vector<wpi::log::DataLogRecord> records,
    wpi::Logger& msgLogger) {
      
    }
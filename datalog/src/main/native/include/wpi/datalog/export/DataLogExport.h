#pragma once

#include <string_view>
#include <vector>
#include "wpi/Logger.h"
#include "wpi/datalog/DataLogReader.h"
#include "wpi/datalog/export/DataLogExportUtils.h"
#include "wpi/raw_ostream.h"
#include "wpi/fs.h"

namespace wpi::log::fileexport {
  class DataLogRecordExporter {
    public:
    static void ExportDataLog(std::string_view fullPath, std::vector<wpi::log::DataLogRecord> records, wpi::Logger& msgLogger);
    static void ExportCSV(std::string_view fullPath, std::vector<wpi::log::DataLogRecord> records, wpi::Logger& msgLogger);
    static void ExportJSON(std::string_view fullPath, std::vector<wpi::log::DataLogRecord> records, wpi::Logger& msgLogger);
  };

  class DataLogCSVExporter {
    public:
     static void Export(std::string inputFilePath, std::string exportFilePath);
    private:
     void WriteValue(wpi::raw_ostream& os, const Entry& entry,
                     const wpi::log::DataLogRecord& record);
     void PrintEscapedCsvString(wpi::raw_ostream& os, std::string_view str);
     fs::path exportFile;
     fs::path inputFile;
   };
}
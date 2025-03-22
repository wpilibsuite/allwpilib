#pragma once
#include <wpi/fs.h>
#include <string>
#include "wpi/datalog/DataLogReader.h"
#include "wpi/datalog/export/DataLogExportUtils.h"
#include "wpi/raw_ostream.h"

namespace wpi::log::fileexport {
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
}  // namespace wpi::log::fileexport
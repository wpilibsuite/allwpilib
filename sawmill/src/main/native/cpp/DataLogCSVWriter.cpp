// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <ctime>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <DataLogExport.h>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <wpi/datalog/DataLogReaderThread.h>
#include <wpi/DenseMap.h>
#include <wpi/MemoryBuffer.h>
#include <wpi/SmallVector.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/fmt/raw_ostream.h>
#include <wpi/fs.h>
#include <wpi/mutex.h>
#include <wpi/print.h>
#include <wpi/raw_ostream.h>

// namespace {
// struct InputFile {
//   explicit InputFile(std::unique_ptr<wpi::DataLogReaderThread> datalog);

//   InputFile(std::string_view filename, std::string_view status)
//       : filename{filename},
//         stem{fs::path{filename}.stem().string()},
//         status{status} {}

//   ~InputFile();

//   std::string filename;
//   std::string stem;
//   std::unique_ptr<wpi::DataLogReaderThread> datalog;
//   std::string status;
//   bool highlight = false;
// };
// }  // namespace

// static std::map<std::string, std::unique_ptr<InputFile>, std::less<>>
//     gInputFiles;
static wpi::mutex gEntriesMutex;
static std::map<std::string, std::unique_ptr<sawmill::Entry>, std::less<>>
    gEntries;
std::atomic_int gExportCount{0};

// InputFile::InputFile(std::unique_ptr<wpi::DataLogReaderThread> datalog_)
//     : filename{datalog_->GetBufferIdentifier()},
//       stem{fs::path{filename}.stem().string()},
//       datalog{std::move(datalog_)} {
//   datalog->sigEntryAdded.connect([this](const wpi::log::StartRecordData& srd)
//   {
//     std::scoped_lock lock{gEntriesMutex};
//     auto it = gEntries.find(srd.name);
//     if (it == gEntries.end()) {
//       it = gEntries.emplace(srd.name, std::make_unique<Entry>(srd)).first;
//     } else {
//       if (it->second->type != srd.type) {
//         it->second->typeConflict = true;
//       }
//       if (it->second->metadata != srd.metadata) {
//         it->second->metadataConflict = true;
//       }
//     }
//     it->second->inputFiles.emplace(this);
//   });
// }

// InputFile::~InputFile() {
//   if (!datalog) {
//     return;
//   }
//   std::scoped_lock lock{gEntriesMutex};
//   bool changed = false;
//   for (auto it = gEntries.begin(); it != gEntries.end();) {
//     it->second->inputFiles.erase(this);
//     if (it->second->inputFiles.empty()) {
//       it = gEntries.erase(it);
//       changed = true;
//     } else {
//       ++it;
//     }
//   }
// }

using namespace sawmill;

static wpi::mutex gExportMutex;
static std::vector<std::string> gExportErrors;

static void PrintEscapedCsvString(wpi::raw_ostream& os, std::string_view str) {
  auto s = str;
  while (!s.empty()) {
    std::string_view fragment;
    std::tie(fragment, s) = wpi::split(s, '"');
    os << fragment;
    if (!s.empty()) {
      os << '"' << '"';
    }
  }
  if (wpi::ends_with(str, '"')) {
    os << '"' << '"';
  }
}

static void ValueToCsv(wpi::raw_ostream& os,
                       const sawmill::DataLogRecord& record) {
  // systemTime needs special handling
  if (record.entryData.name == "systemTime" &&
      record.entryData.type == "int64") {
    int64_t val;
    if (record.dataLogRecord.GetInteger(&val)) {
      std::time_t timeval = val / 1000000;
      wpi::print(os, "{:%Y-%m-%d %H:%M:%S}.{:06}", *std::localtime(&timeval),
                 val % 1000000);
      return;
    }
  } else if (record.entryData.type == "double") {
    double val;
    if (record.dataLogRecord.GetDouble(&val)) {
      wpi::print(os, "{}", val);
      return;
    }
  } else if (record.entryData.type == "int64" ||
             record.entryData.type == "int") {
    // support "int" for compatibility with old NT4 datalogs
    int64_t val;
    if (record.dataLogRecord.GetInteger(&val)) {
      wpi::print(os, "{}", val);
      return;
    }
  } else if (record.entryData.type == "string" ||
             record.entryData.type == "json") {
    std::string_view val;
    record.dataLogRecord.GetString(&val);
    os << '"';
    PrintEscapedCsvString(os, val);
    os << '"';
    return;
  } else if (record.entryData.type == "boolean") {
    bool val;
    if (record.dataLogRecord.GetBoolean(&val)) {
      wpi::print(os, "{}", val);
      return;
    }
  } else if (record.entryData.type == "boolean[]") {
    std::vector<int> val;
    if (record.dataLogRecord.GetBooleanArray(&val)) {
      wpi::print(os, "{}", fmt::join(val, ";"));
      return;
    }
  } else if (record.entryData.type == "double[]") {
    std::vector<double> val;
    if (record.dataLogRecord.GetDoubleArray(&val)) {
      wpi::print(os, "{}", fmt::join(val, ";"));
      return;
    }
  } else if (record.entryData.type == "float[]") {
    std::vector<float> val;
    if (record.dataLogRecord.GetFloatArray(&val)) {
      wpi::print(os, "{}", fmt::join(val, ";"));
      return;
    }
  } else if (record.entryData.type == "int64[]") {
    std::vector<int64_t> val;
    if (record.dataLogRecord.GetIntegerArray(&val)) {
      wpi::print(os, "{}", fmt::join(val, ";"));
      return;
    }
  } else if (record.entryData.type == "string[]") {
    std::vector<std::string_view> val;
    if (record.dataLogRecord.GetStringArray(&val)) {
      os << '"';
      bool first = true;
      for (auto&& v : val) {
        if (!first) {
          os << ';';
        }
        first = false;
        PrintEscapedCsvString(os, v);
      }
      os << '"';
      return;
    }
  }
  wpi::print(os, "<invalid>");
}

// static void ValueToCsv(wpi::raw_ostream& os, const Entry& entry,
//                        const wpi::log::DataLogRecord& record) {
//   // handle systemTime specially
//   if (entry.name == "systemTime" && entry.type == "int64") {
//     int64_t val;
//     if (record.GetInteger(&val)) {
//       std::time_t timeval = val / 1000000;
//       wpi::print(os, "{:%Y-%m-%d %H:%M:%S}.{:06}", *std::localtime(&timeval),
//                  val % 1000000);
//       return;
//     }
//   } else if (entry.type == "double") {
//     double val;
//     if (record.GetDouble(&val)) {
//       wpi::print(os, "{}", val);
//       return;
//     }
//   } else if (entry.type == "int64" || entry.type == "int") {
//     // support "int" for compatibility with old NT4 datalogs
//     int64_t val;
//     if (record.GetInteger(&val)) {
//       wpi::print(os, "{}", val);
//       return;
//     }
//   } else if (entry.type == "string" || entry.type == "json") {
//     std::string_view val;
//     record.GetString(&val);
//     os << '"';
//     PrintEscapedCsvString(os, val);
//     os << '"';
//     return;
//   } else if (entry.type == "boolean") {
//     bool val;
//     if (record.GetBoolean(&val)) {
//       wpi::print(os, "{}", val);
//       return;
//     }
//   } else if (entry.type == "boolean[]") {
//     std::vector<int> val;
//     if (record.GetBooleanArray(&val)) {
//       wpi::print(os, "{}", fmt::join(val, ";"));
//       return;
//     }
//   } else if (entry.type == "double[]") {
//     std::vector<double> val;
//     if (record.GetDoubleArray(&val)) {
//       wpi::print(os, "{}", fmt::join(val, ";"));
//       return;
//     }
//   } else if (entry.type == "float[]") {
//     std::vector<float> val;
//     if (record.GetFloatArray(&val)) {
//       wpi::print(os, "{}", fmt::join(val, ";"));
//       return;
//     }
//   } else if (entry.type == "int64[]") {
//     std::vector<int64_t> val;
//     if (record.GetIntegerArray(&val)) {
//       wpi::print(os, "{}", fmt::join(val, ";"));
//       return;
//     }
//   } else if (entry.type == "string[]") {
//     std::vector<std::string_view> val;
//     if (record.GetStringArray(&val)) {
//       os << '"';
//       bool first = true;
//       for (auto&& v : val) {
//         if (!first) {
//           os << ';';
//         }
//         first = false;
//         PrintEscapedCsvString(os, v);
//       }
//       os << '"';
//       return;
//     }
//   }
//   wpi::print(os, "<invalid>");
// }

void ExportCsvFile(wpi::raw_ostream& os, int style, bool printControlRecords,
                   std::vector<sawmill::DataLogRecord> records,
                   std::map<int, sawmill::Entry, std::less<>> entryMap) {
  // print header
  if (style == 0) {
    os << "Timestamp,Name,Value\n";
  } else if (style == 1) {
    // scan for exported fields for this file to print header and assign columns
    os << "Timestamp";
    int columnNum = 0;
    for (std::pair<const int, sawmill::Entry>& entry : entryMap) {
      os << ',' << '"';
      PrintEscapedCsvString(os, entry.second.name);
      os << '"';
      entry.second.column = columnNum++;
    }
    os << '\n';
  }

  for (sawmill::DataLogRecord record : records) {
    // if this is a control record and we dont want to print those, skip
    if (record.dataLogRecord.IsControl() && !printControlRecords) {
      continue;
    }

    if (style == 0) {
      wpi::print(os, "{},\"", record.dataLogRecord.GetTimestamp() / 1000000.0);
      PrintEscapedCsvString(os, record.entryData.name);
      os << '"' << ',';
      ValueToCsv(os, record);
      os << '\n';
    } else if (style == 1) {
      wpi::print(os, "{},", record.dataLogRecord.GetTimestamp() / 1000000.0);
      for (int i = 0; i < record.entryData.column; ++i) {
        os << ',';
      }
      ValueToCsv(os, record);
      os << '\n';
    }
  }
}

// static void ExportCsvFile(InputFile& f, wpi::raw_ostream& os, int style) {
//   // header
//   if (style == 0) {
//     os << "Timestamp,Name,Value\n";
//   } else if (style == 1) {
//     // scan for exported fields for this file to print header and assign
//     columns os << "Timestamp"; int columnNum = 0; for (auto&& entry :
//     gEntries) {
//       if (entry.second->selected &&
//           entry.second->inputFiles.find(&f) !=
//           entry.second->inputFiles.end()) {
//         os << ',' << '"';
//         PrintEscapedCsvString(os, entry.first);
//         os << '"';
//         entry.second->column = columnNum++;
//       } else {
//         entry.second->column = -1;
//       }
//     }
//     os << '\n';
//   }

//   wpi::DenseMap<int, Entry*> nameMap;
//   for (wpi::log::DataLogRecord record : f.datalog->GetReader()) {
//     if (record.IsStart()) {
//       wpi::log::StartRecordData data;
//       if (record.GetStartData(&data)) {
//         auto it = gEntries.find(data.name);
//         if (it != gEntries.end() && it->second->selected) {
//           nameMap[data.entry] = it->second.get();
//         }
//       }
//     } else if (record.IsFinish()) {
//       int entry;
//       if (record.GetFinishEntry(&entry)) {
//         nameMap.erase(entry);
//       }
//     } else if (!record.IsControl()) {
//       auto entryIt = nameMap.find(record.GetEntry());
//       if (entryIt == nameMap.end()) {
//         continue;
//       }
//       Entry* entry = entryIt->second;

//       if (style == 0) {
//         wpi::print(os, "{},\"", record.GetTimestamp() / 1000000.0);
//         PrintEscapedCsvString(os, entry->name);
//         os << '"' << ',';
//         ValueToCsv(os, record);
//         os << '\n';
//       } else if (style == 1 && entry->column != -1) {
//         wpi::print(os, "{},", record.GetTimestamp() / 1000000.0);
//         for (int i = 0; i < entry->column; ++i) {
//           os << ',';
//         }
//         ValueToCsv(os, *entry, record);
//         os << '\n';
//       }
//     }
//   }
// }

// static void ExportCsv(std::string_view outputFolder, int style) {
//   fs::path outPath{outputFolder};
//   for (auto&& f : gInputFiles) {
//     if (f.second->datalog) {
//       std::error_code ec;
//       auto of = fs::OpenFileForWrite(
//           outPath / fs::path{f.first}.replace_extension("csv"), ec,
//           fs::CD_CreateNew, fs::OF_Text);
//       if (ec) {
//         std::scoped_lock lock{gExportMutex};
//         gExportErrors.emplace_back(
//             fmt::format("{}: {}", f.first, ec.message()));
//         ++gExportCount;
//         continue;
//       }
//       wpi::raw_fd_ostream os{fs::FileToFd(of, ec, fs::OF_Text), true};
//       ExportCsvFile(*f.second, os, style);
//     }
//     ++gExportCount;
//   }
// }

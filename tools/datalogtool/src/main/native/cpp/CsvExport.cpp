// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CsvExport.hpp"

#include <chrono>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "CsvTable.hpp"
#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/fmt/raw_ostream.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {
static void PrintEscapedCsvString(wpi::util::raw_ostream& os,
                                  std::string_view str) {
  auto s = str;
  while (!s.empty()) {
    std::string_view fragment;
    std::tie(fragment, s) = wpi::util::split(s, '"');
    os << fragment;
    if (!s.empty()) {
      os << '"' << '"';
    }
  }
  if (wpi::util::ends_with(str, '"')) {
    os << '"' << '"';
  }
}

static void ValueToCsv(wpi::util::raw_ostream& os,
                       const wpi::log::StartRecordData& entry,
                       const wpi::log::DataLogRecord& record) {
  // handle systemTime specially
  if (entry.name == "systemTime" && entry.type == "int64") {
    int64_t val;
    if (record.GetInteger(&val)) {
      auto timeval =
          std::chrono::system_clock::time_point(std::chrono::microseconds(val));
      wpi::util::print(os, "{:%Y-%m-%d %H:%M:%OS}.{:06}", timeval,
                       val % 1000000);
      return;
    }
  } else if (entry.type == "double") {
    double val;
    if (record.GetDouble(&val)) {
      wpi::util::print(os, "{}", val);
      return;
    }
  } else if (entry.type == "float") {
    float val;
    if (record.GetFloat(&val)) {
      wpi::util::print(os, "{}", val);
      return;
    }
  } else if (entry.type == "int64" || entry.type == "int") {
    // support "int" for compatibility with old NT4 datalogs
    int64_t val;
    if (record.GetInteger(&val)) {
      wpi::util::print(os, "{}", val);
      return;
    }
  } else if (entry.type == "string" || entry.type == "json") {
    std::string_view val;
    record.GetString(&val);
    os << '"';
    PrintEscapedCsvString(os, val);
    os << '"';
    return;
  } else if (entry.type == "boolean") {
    bool val;
    if (record.GetBoolean(&val)) {
      wpi::util::print(os, "{}", val);
      return;
    }
  } else if (entry.type == "boolean[]") {
    std::vector<int> val;
    if (record.GetBooleanArray(&val)) {
      wpi::util::print(os, "{}", wpi::util::join(val, ";"));
      return;
    }
  } else if (entry.type == "double[]") {
    std::vector<double> val;
    if (record.GetDoubleArray(&val)) {
      wpi::util::print(os, "{}", wpi::util::join(val, ";"));
      return;
    }
  } else if (entry.type == "float[]") {
    std::vector<float> val;
    if (record.GetFloatArray(&val)) {
      wpi::util::print(os, "{}", wpi::util::join(val, ";"));
      return;
    }
  } else if (entry.type == "int64[]") {
    std::vector<int64_t> val;
    if (record.GetIntegerArray(&val)) {
      wpi::util::print(os, "{}", wpi::util::join(val, ";"));
      return;
    }
  } else if (entry.type == "string[]") {
    std::vector<std::string_view> val;
    if (record.GetStringArray(&val)) {
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
  wpi::util::print(os, "<invalid>");
}

}  // namespace

void dlt::WriteCsvString(wpi::util::raw_ostream& os, std::string_view value) {
  os << '"';
  PrintEscapedCsvString(os, value);
  os << '"';
}

void dlt::ExportCsv(const wpi::log::DataLogReader& reader,
                    wpi::util::raw_ostream& os, CsvStyle style,
                    int64_t timestampFuzziness, const EntryFilter& selected) {
  std::map<std::string_view, size_t> columns;
  if (style == CsvStyle::LIST) {
    os << "Timestamp,Name,Value\n";
  } else {
    for (auto&& record : reader) {
      wpi::log::StartRecordData data;
      if (record.IsStart() && record.GetStartData(&data) &&
          (!selected || selected(data.name))) {
        columns.emplace(data.name, 0);
      }
    }
    os << "Timestamp";
    size_t column = 0;
    for (auto& [name, index] : columns) {
      index = column++;
      os << ',';
      WriteCsvString(os, name);
    }
    os << '\n';
  }

  CsvTable table{os, columns.size(), timestampFuzziness};
  wpi::util::DenseMap<int, wpi::log::StartRecordData> entries;
  for (auto&& record : reader) {
    if (record.IsStart()) {
      wpi::log::StartRecordData data;
      if (record.GetStartData(&data)) {
        // Entry IDs may be reused, including for an unselected name or new
        // type.
        entries.erase(data.entry);
        if (!selected || selected(data.name)) {
          entries[data.entry] = data;
        }
      }
    } else if (record.IsFinish()) {
      int entry;
      if (record.GetFinishEntry(&entry)) {
        entries.erase(entry);
      }
    } else if (!record.IsControl()) {
      auto it = entries.find(record.GetEntry());
      if (it == entries.end()) {
        continue;
      }
      const auto& entry = it->second;
      if (style == CsvStyle::LIST) {
        wpi::util::print(os, "{},", record.GetTimestamp() / 1'000'000'000.0);
        WriteCsvString(os, entry.name);
        os << ',';
        ValueToCsv(os, entry, record);
        os << '\n';
      } else {
        std::string value;
        wpi::util::raw_string_ostream valueStream{value};
        ValueToCsv(valueStream, entry, record);
        table.Add(record.GetTimestamp(), columns.at(entry.name), value);
      }
    }
  }
  table.Flush();
}

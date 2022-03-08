// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <ctime>

#include "fmt/chrono.h"
#include "fmt/format.h"
#include "wpi/DataLogReader.h"
#include "wpi/DenseMap.h"
#include "wpi/MemoryBuffer.h"

int main(int argc, const char** argv) {
  if (argc != 2) {
    fmt::print(stderr, "Usage: printlog <file>\n");
    return EXIT_FAILURE;
  }
  std::error_code ec;
  wpi::log::DataLogReader reader{wpi::MemoryBuffer::GetFile(argv[1], ec)};
  if (ec) {
    fmt::print(stderr, "could not open file: {}\n", ec.message());
    return EXIT_FAILURE;
  }
  if (!reader) {
    fmt::print(stderr, "not a log file\n");
    return EXIT_FAILURE;
  }

  wpi::DenseMap<int, wpi::log::StartRecordData> entries;
  for (auto&& record : reader) {
    if (record.IsStart()) {
      wpi::log::StartRecordData data;
      if (record.GetStartData(&data)) {
        fmt::print("Start({}, name='{}', type='{}', metadata='{}') [{}]\n",
                   data.entry, data.name, data.type, data.metadata,
                   record.GetTimestamp() / 1000000.0);
        if (entries.find(data.entry) != entries.end()) {
          fmt::print("...DUPLICATE entry ID, overriding\n");
        }
        entries[data.entry] = data;
      } else {
        fmt::print("Start(INVALID)\n");
      }
    } else if (record.IsFinish()) {
      int entry;
      if (record.GetFinishEntry(&entry)) {
        fmt::print("Finish({}) [{}]\n", entry,
                   record.GetTimestamp() / 1000000.0);
        auto it = entries.find(entry);
        if (it == entries.end()) {
          fmt::print("...ID not found\n");
        } else {
          entries.erase(it);
        }
      } else {
        fmt::print("Finish(INVALID)\n");
      }
    } else if (record.IsSetMetadata()) {
      wpi::log::MetadataRecordData data;
      if (record.GetSetMetadataData(&data)) {
        fmt::print("SetMetadata({}, '{}') [{}]\n", data.entry, data.metadata,
                   record.GetTimestamp() / 1000000.0);
        auto it = entries.find(data.entry);
        if (it == entries.end()) {
          fmt::print("...ID not found\n");
        } else {
          it->second.metadata = data.metadata;
        }
      } else {
        fmt::print("SetMetadata(INVALID)\n");
      }
    } else if (record.IsControl()) {
      fmt::print("Unrecognized control record\n");
    } else {
      fmt::print("Data({}, size={}) ", record.GetEntry(), record.GetSize());
      auto entry = entries.find(record.GetEntry());
      if (entry == entries.end()) {
        fmt::print("<ID not found>\n");
        continue;
      }
      fmt::print("<name='{}', type='{}'> [{}]\n", entry->second.name,
                 entry->second.type, record.GetTimestamp() / 1000000.0);

      // handle systemTime specially
      if (entry->second.name == "systemTime" && entry->second.type == "int64") {
        int64_t val;
        if (record.GetInteger(&val)) {
          std::time_t timeval = val / 1000000;
          fmt::print("  {:%Y-%m-%d %H:%M:%S}.{:06}\n",
                     *std::localtime(&timeval), val % 1000000);
        } else {
          fmt::print("  invalid\n");
        }
        continue;
      }

      if (entry->second.type == "double") {
        double val;
        if (record.GetDouble(&val)) {
          fmt::print("  {}\n", val);
        } else {
          fmt::print("  invalid\n");
        }
      } else if (entry->second.type == "int64") {
        int64_t val;
        if (record.GetInteger(&val)) {
          fmt::print("  {}\n", val);
        } else {
          fmt::print("  invalid\n");
        }
      } else if (entry->second.type == "string" ||
                 entry->second.type == "json") {
        std::string_view val;
        record.GetString(&val);
        fmt::print("  '{}'\n", val);
      } else if (entry->second.type == "boolean") {
        bool val;
        if (record.GetBoolean(&val)) {
          fmt::print("  {}\n", val);
        } else {
          fmt::print("  invalid\n");
        }
      } else if (entry->second.type == "boolean[]") {
        std::vector<int> val;
        if (record.GetBooleanArray(&val)) {
          fmt::print("  {}\n", fmt::join(val, ", "));
        } else {
          fmt::print("  invalid\n");
        }
      } else if (entry->second.type == "double[]") {
        std::vector<double> val;
        if (record.GetDoubleArray(&val)) {
          fmt::print("  {}\n", fmt::join(val, ", "));
        } else {
          fmt::print("  invalid\n");
        }
      } else if (entry->second.type == "float[]") {
        std::vector<float> val;
        if (record.GetFloatArray(&val)) {
          fmt::print("  {}\n", fmt::join(val, ", "));
        } else {
          fmt::print("  invalid\n");
        }
      } else if (entry->second.type == "int64[]") {
        std::vector<int64_t> val;
        if (record.GetIntegerArray(&val)) {
          fmt::print("  {}\n", fmt::join(val, ", "));
        } else {
          fmt::print("  invalid\n");
        }
      } else if (entry->second.type == "string[]") {
        std::vector<std::string_view> val;
        if (record.GetStringArray(&val)) {
          fmt::print("  {}\n", fmt::join(val, ", "));
        } else {
          fmt::print("  invalid\n");
        }
      }
    }
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <ctime>
#include <utility>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include "wpi/DataLogReader.h"
#include "wpi/DenseMap.h"
#include "wpi/MemoryBuffer.h"
#include "wpi/print.h"

int main(int argc, const char** argv) {
  if (argc != 2) {
    wpi::print(stderr, "Usage: printlog <file>\n");
    return EXIT_FAILURE;
  }
  auto fileBuffer = wpi::MemoryBuffer::GetFile(argv[1]);
  if (!fileBuffer) {
    wpi::print(stderr, "could not open file: {}\n",
               fileBuffer.error().message());
    return EXIT_FAILURE;
  }
  wpi::log::DataLogReader reader{std::move(*fileBuffer)};
  if (!reader) {
    wpi::print(stderr, "not a log file\n");
    return EXIT_FAILURE;
  }

  wpi::DenseMap<int, wpi::log::StartRecordData> entries;
  for (auto&& record : reader) {
    if (record.IsStart()) {
      wpi::log::StartRecordData data;
      if (record.GetStartData(&data)) {
        wpi::print("Start({}, name='{}', type='{}', metadata='{}') [{}]\n",
                   data.entry, data.name, data.type, data.metadata,
                   record.GetTimestamp() / 1000000.0);
        if (entries.find(data.entry) != entries.end()) {
          wpi::print("...DUPLICATE entry ID, overriding\n");
        }
        entries[data.entry] = data;
      } else {
        wpi::print("Start(INVALID)\n");
      }
    } else if (record.IsFinish()) {
      int entry;
      if (record.GetFinishEntry(&entry)) {
        wpi::print("Finish({}) [{}]\n", entry,
                   record.GetTimestamp() / 1000000.0);
        auto it = entries.find(entry);
        if (it == entries.end()) {
          wpi::print("...ID not found\n");
        } else {
          entries.erase(it);
        }
      } else {
        wpi::print("Finish(INVALID)\n");
      }
    } else if (record.IsSetMetadata()) {
      wpi::log::MetadataRecordData data;
      if (record.GetSetMetadataData(&data)) {
        wpi::print("SetMetadata({}, '{}') [{}]\n", data.entry, data.metadata,
                   record.GetTimestamp() / 1000000.0);
        auto it = entries.find(data.entry);
        if (it == entries.end()) {
          wpi::print("...ID not found\n");
        } else {
          it->second.metadata = data.metadata;
        }
      } else {
        wpi::print("SetMetadata(INVALID)\n");
      }
    } else if (record.IsControl()) {
      wpi::print("Unrecognized control record\n");
    } else {
      wpi::print("Data({}, size={}) ", record.GetEntry(), record.GetSize());
      auto entry = entries.find(record.GetEntry());
      if (entry == entries.end()) {
        wpi::print("<ID not found>\n");
        continue;
      }
      wpi::print("<name='{}', type='{}'> [{}]\n", entry->second.name,
                 entry->second.type, record.GetTimestamp() / 1000000.0);

      // handle systemTime specially
      if (entry->second.name == "systemTime" && entry->second.type == "int64") {
        int64_t val;
        if (record.GetInteger(&val)) {
          std::time_t timeval = val / 1000000;
          wpi::print("  {:%Y-%m-%d %H:%M:%S}.{:06}\n",
                     *std::localtime(&timeval), val % 1000000);
        } else {
          wpi::print("  invalid\n");
        }
        continue;
      }

      if (entry->second.type == "double") {
        double val;
        if (record.GetDouble(&val)) {
          wpi::print("  {}\n", val);
        } else {
          wpi::print("  invalid\n");
        }
      } else if (entry->second.type == "int64") {
        int64_t val;
        if (record.GetInteger(&val)) {
          wpi::print("  {}\n", val);
        } else {
          wpi::print("  invalid\n");
        }
      } else if (entry->second.type == "string" ||
                 entry->second.type == "json") {
        std::string_view val;
        record.GetString(&val);
        wpi::print("  '{}'\n", val);
      } else if (entry->second.type == "boolean") {
        bool val;
        if (record.GetBoolean(&val)) {
          wpi::print("  {}\n", val);
        } else {
          wpi::print("  invalid\n");
        }
      } else if (entry->second.type == "boolean[]") {
        std::vector<int> val;
        if (record.GetBooleanArray(&val)) {
          wpi::print("  {}\n", fmt::join(val, ", "));
        } else {
          wpi::print("  invalid\n");
        }
      } else if (entry->second.type == "double[]") {
        std::vector<double> val;
        if (record.GetDoubleArray(&val)) {
          wpi::print("  {}\n", fmt::join(val, ", "));
        } else {
          wpi::print("  invalid\n");
        }
      } else if (entry->second.type == "float[]") {
        std::vector<float> val;
        if (record.GetFloatArray(&val)) {
          wpi::print("  {}\n", fmt::join(val, ", "));
        } else {
          wpi::print("  invalid\n");
        }
      } else if (entry->second.type == "int64[]") {
        std::vector<int64_t> val;
        if (record.GetIntegerArray(&val)) {
          wpi::print("  {}\n", fmt::join(val, ", "));
        } else {
          wpi::print("  invalid\n");
        }
      } else if (entry->second.type == "string[]") {
        std::vector<std::string_view> val;
        if (record.GetStringArray(&val)) {
          wpi::print("  {}\n", fmt::join(val, ", "));
        } else {
          wpi::print("  invalid\n");
        }
      }
    }
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/support/DataLogReaderThread.h"

#include <utility>

#include <fmt/format.h>
#include <wpi/StringExtras.h>

using namespace glass;

DataLogReaderThread::~DataLogReaderThread() {
  if (m_thread.joinable()) {
    m_active = false;
    m_thread.join();
  }
}

void DataLogReaderThread::ReadMain() {
  wpi::SmallDenseMap<
      int, std::pair<DataLogReaderEntry*, std::span<const uint8_t>>, 8>
      schemaEntries;

  for (auto recordIt = m_reader.begin(), recordEnd = m_reader.end();
       recordIt != recordEnd; ++recordIt) {
    auto& record = *recordIt;
    if (!m_active) {
      break;
    }
    ++m_numRecords;
    if (record.IsStart()) {
      DataLogReaderEntry data;
      if (record.GetStartData(&data)) {
        std::scoped_lock lock{m_mutex};
        auto& entryPtr = m_entriesById[data.entry];
        if (entryPtr) {
          fmt::print("...DUPLICATE entry ID, overriding\n");
        }
        auto [it, isNew] = m_entriesByName.emplace(data.name, data);
        if (isNew) {
          it->second.ranges.emplace_back(recordIt, recordEnd);
        }
        entryPtr = &it->second;
        if (data.type == "structschema" ||
            data.type == "proto:FileDescriptorProto") {
          schemaEntries.try_emplace(data.entry, entryPtr,
                                    std::span<const uint8_t>{});
        }
        sigEntryAdded(data);
      } else {
        fmt::print("Start(INVALID)\n");
      }
    } else if (record.IsFinish()) {
      int entry;
      if (record.GetFinishEntry(&entry)) {
        std::scoped_lock lock{m_mutex};
        auto it = m_entriesById.find(entry);
        if (it == m_entriesById.end()) {
          fmt::print("...ID not found\n");
        } else {
          it->second->ranges.back().m_end = recordIt;
          m_entriesById.erase(it);
        }
      } else {
        fmt::print("Finish(INVALID)\n");
      }
    } else if (record.IsSetMetadata()) {
      wpi::log::MetadataRecordData data;
      if (record.GetSetMetadataData(&data)) {
        std::scoped_lock lock{m_mutex};
        auto it = m_entriesById.find(data.entry);
        if (it == m_entriesById.end()) {
          fmt::print("...ID not found\n");
        } else {
          it->second->metadata = data.metadata;
        }
      } else {
        fmt::print("SetMetadata(INVALID)\n");
      }
    } else if (record.IsControl()) {
      fmt::print("Unrecognized control record\n");
    } else {
      auto it = schemaEntries.find(record.GetEntry());
      if (it != schemaEntries.end()) {
        it->second.second = record.GetRaw();
      }
    }
  }

  // build schema databases
  for (auto&& schemaPair : schemaEntries) {
    auto name = schemaPair.second.first->name;
    auto data = schemaPair.second.second;
    if (data.empty()) {
      continue;
    }
    if (wpi::starts_with(name, "NT:")) {
      name = wpi::drop_front(name, 3);
    }
    if (wpi::starts_with(name, "/.schema/struct:")) {
      auto typeStr = wpi::drop_front(name, 16);
      std::string_view schema{reinterpret_cast<const char*>(data.data()),
                              data.size()};
      std::string err;
      auto desc = m_structDb.Add(typeStr, schema, &err);
      if (!desc) {
        fmt::print("could not decode struct '{}' schema '{}': {}\n", name,
                   schema, err);
      }
    } else if (wpi::starts_with(name, "/.schema/proto:")) {
      // protobuf descriptor handling
      auto filename = wpi::drop_front(name, 15);
      if (!m_protoDb.Add(filename, data)) {
        fmt::print("could not decode protobuf '{}' filename '{}'\n", name,
                   filename);
      }
    }
  }

  sigDone();
  m_done = true;
}

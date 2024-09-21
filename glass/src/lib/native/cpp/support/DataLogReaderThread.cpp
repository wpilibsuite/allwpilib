// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/support/DataLogReaderThread.h"

#include <string>
#include <utility>

#include <wpi/StringExtras.h>
#include <wpi/print.h>

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
          wpi::print("...DUPLICATE entry ID, overriding\n");
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
        wpi::print("Start(INVALID)\n");
      }
    } else if (record.IsFinish()) {
      int entry;
      if (record.GetFinishEntry(&entry)) {
        std::scoped_lock lock{m_mutex};
        auto it = m_entriesById.find(entry);
        if (it == m_entriesById.end()) {
          wpi::print("...ID not found\n");
        } else {
          it->second->ranges.back().m_end = recordIt;
          m_entriesById.erase(it);
        }
      } else {
        wpi::print("Finish(INVALID)\n");
      }
    } else if (record.IsSetMetadata()) {
      wpi::log::MetadataRecordData data;
      if (record.GetSetMetadataData(&data)) {
        std::scoped_lock lock{m_mutex};
        auto it = m_entriesById.find(data.entry);
        if (it == m_entriesById.end()) {
          wpi::print("...ID not found\n");
        } else {
          it->second->metadata = data.metadata;
        }
      } else {
        wpi::print("SetMetadata(INVALID)\n");
      }
    } else if (record.IsControl()) {
      wpi::print("Unrecognized control record\n");
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
    if (auto strippedName = wpi::remove_prefix(name, "NT:")) {
      name = *strippedName;
    }
    if (auto typeStr = wpi::remove_prefix(name, "/.schema/struct:")) {
      std::string_view schema{reinterpret_cast<const char*>(data.data()),
                              data.size()};
      std::string err;
      auto desc = m_structDb.Add(*typeStr, schema, &err);
      if (!desc) {
        wpi::print("could not decode struct '{}' schema '{}': {}\n", name,
                   schema, err);
      }
    } else if (auto filename = wpi::remove_prefix(name, "/.schema/proto:")) {
#ifndef NO_PROTOBUF
      // protobuf descriptor handling
      if (!m_protoDb.Add(*filename, data)) {
        wpi::print("could not decode protobuf '{}' filename '{}'\n", name,
                   *filename);
      }
#endif
    }
  }

  sigDone();
  m_done = true;
}

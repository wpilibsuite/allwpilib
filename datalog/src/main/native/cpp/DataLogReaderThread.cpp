// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/datalog/DataLogReaderThread.hpp"

#include <string>
#include <utility>

#include "wpi/util/StringExtras.hpp"
#include "wpi/util/print.hpp"

using namespace wpi::log;

DataLogReaderThread::~DataLogReaderThread() {
  if (m_thread.joinable()) {
    m_active = false;
    m_thread.join();
  }
}

void DataLogReaderThread::ReadMain() {
  wpi::util::SmallDenseMap<
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
          wpi::util::print("...DUPLICATE entry ID, overriding\n");
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
        wpi::util::print("Start(INVALID)\n");
      }
    } else if (record.IsFinish()) {
      int entry;
      if (record.GetFinishEntry(&entry)) {
        std::scoped_lock lock{m_mutex};
        auto it = m_entriesById.find(entry);
        if (it == m_entriesById.end()) {
          wpi::util::print("...ID not found\n");
        } else {
          it->second->ranges.back().m_end = recordIt;
          m_entriesById.erase(it);
        }
      } else {
        wpi::util::print("Finish(INVALID)\n");
      }
    } else if (record.IsSetMetadata()) {
      wpi::log::MetadataRecordData data;
      if (record.GetSetMetadataData(&data)) {
        std::scoped_lock lock{m_mutex};
        auto it = m_entriesById.find(data.entry);
        if (it == m_entriesById.end()) {
          wpi::util::print("...ID not found\n");
        } else {
          it->second->metadata = data.metadata;
        }
      } else {
        wpi::util::print("SetMetadata(INVALID)\n");
      }
    } else if (record.IsControl()) {
      wpi::util::print("Unrecognized control record\n");
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
    if (auto strippedName = wpi::util::remove_prefix(name, "NT:")) {
      name = *strippedName;
    }
    if (auto typeStr = wpi::util::remove_prefix(name, "/.schema/struct:")) {
      std::string_view schema{reinterpret_cast<const char*>(data.data()),
                              data.size()};
      std::string err;
      auto desc = m_structDb.Add(*typeStr, schema, &err);
      if (!desc) {
        wpi::util::print("could not decode struct '{}' schema '{}': {}\n", name,
                   schema, err);
      }
    } else if (auto filename = wpi::util::remove_prefix(name, "/.schema/proto:")) {
      // protobuf descriptor handling
      upb_Status status;
      status.ok = true;
      upb_DefPool_AddFile(
          m_protoPool,
          google_protobuf_FileDescriptorProto_parse(
              reinterpret_cast<const char*>(data.data()), data.size(), m_arena),
          &status);
      if (!status.ok) {
        wpi::util::print("could not decode protobuf '{}' filename '{}'\n", name,
                   *filename);
      }
    }
  }

  sigDone();
  m_done = true;
}

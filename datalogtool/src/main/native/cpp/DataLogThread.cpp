// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DataLogThread.h"

#include <fmt/format.h>

DataLogThread::~DataLogThread() {
  if (m_thread.joinable()) {
    m_active = false;
    m_thread.join();
  }
}

void DataLogThread::ReadMain() {
  for (auto record : m_reader) {
    if (!m_active) {
      break;
    }
    ++m_numRecords;
    if (record.IsStart()) {
      wpi::log::StartRecordData data;
      if (record.GetStartData(&data)) {
        std::scoped_lock lock{m_mutex};
        if (m_entries.find(data.entry) != m_entries.end()) {
          fmt::print("...DUPLICATE entry ID, overriding\n");
        }
        m_entries[data.entry] = data;
        m_entryNames.emplace(data.name, data);
        sigEntryAdded(data);
      } else {
        fmt::print("Start(INVALID)\n");
      }
    } else if (record.IsFinish()) {
      int entry;
      if (record.GetFinishEntry(&entry)) {
        std::scoped_lock lock{m_mutex};
        auto it = m_entries.find(entry);
        if (it == m_entries.end()) {
          fmt::print("...ID not found\n");
        } else {
          m_entries.erase(it);
        }
      } else {
        fmt::print("Finish(INVALID)\n");
      }
    } else if (record.IsSetMetadata()) {
      wpi::log::MetadataRecordData data;
      if (record.GetSetMetadataData(&data)) {
        std::scoped_lock lock{m_mutex};
        auto it = m_entries.find(data.entry);
        if (it == m_entries.end()) {
          fmt::print("...ID not found\n");
        } else {
          it->second.metadata = data.metadata;
          auto nameIt = m_entryNames.find(it->second.name);
          if (nameIt != m_entryNames.end()) {
            nameIt->second.metadata = data.metadata;
          }
        }
      } else {
        fmt::print("SetMetadata(INVALID)\n");
      }
    } else if (record.IsControl()) {
      fmt::print("Unrecognized control record\n");
    }
  }

  sigDone();
  m_done = true;
}

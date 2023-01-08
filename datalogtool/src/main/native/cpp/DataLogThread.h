// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include <wpi/DataLogReader.h>
#include <wpi/DenseMap.h>
#include <wpi/Signal.h>
#include <wpi/mutex.h>

class DataLogThread {
 public:
  explicit DataLogThread(wpi::log::DataLogReader reader)
      : m_reader{std::move(reader)}, m_thread{[=, this] { ReadMain(); }} {}
  ~DataLogThread();

  bool IsDone() const { return m_done; }
  std::string_view GetBufferIdentifier() const {
    return m_reader.GetBufferIdentifier();
  }
  unsigned int GetNumRecords() const { return m_numRecords; }
  unsigned int GetNumEntries() const {
    std::scoped_lock lock{m_mutex};
    return m_entryNames.size();
  }

  // Passes wpi::log::StartRecordData to func
  template <typename T>
  void ForEachEntryName(T&& func) {
    std::scoped_lock lock{m_mutex};
    for (auto&& kv : m_entryNames) {
      func(kv.second);
    }
  }

  wpi::log::StartRecordData GetEntry(std::string_view name) const {
    std::scoped_lock lock{m_mutex};
    auto it = m_entryNames.find(name);
    if (it == m_entryNames.end()) {
      return {};
    }
    return it->second;
  }

  const wpi::log::DataLogReader& GetReader() const { return m_reader; }

  // note: these are called on separate thread
  wpi::sig::Signal_mt<const wpi::log::StartRecordData&> sigEntryAdded;
  wpi::sig::Signal_mt<> sigDone;

 private:
  void ReadMain();

  wpi::log::DataLogReader m_reader;
  mutable wpi::mutex m_mutex;
  std::atomic_bool m_active{true};
  std::atomic_bool m_done{false};
  std::atomic<unsigned int> m_numRecords{0};
  std::map<std::string, wpi::log::StartRecordData, std::less<>> m_entryNames;
  wpi::DenseMap<int, wpi::log::StartRecordData> m_entries;
  std::thread m_thread;
};

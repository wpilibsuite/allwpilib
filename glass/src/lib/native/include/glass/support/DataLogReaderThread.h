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
#include <vector>

#include <wpi/DataLogReader.h>
#include <wpi/DenseMap.h>
#include <wpi/Signal.h>
#include <wpi/mutex.h>
#include <wpi/struct/DynamicStruct.h>

#ifndef NO_PROTOBUF
#include <wpi/protobuf/ProtobufMessageDatabase.h>
#endif

namespace glass {

class DataLogReaderRange {
 public:
  DataLogReaderRange(wpi::log::DataLogReader::iterator begin,
                     wpi::log::DataLogReader::iterator end)
      : m_begin{begin}, m_end{end} {}

  wpi::log::DataLogReader::iterator begin() const { return m_begin; }
  wpi::log::DataLogReader::iterator end() const { return m_end; }

  wpi::log::DataLogReader::iterator m_begin;
  wpi::log::DataLogReader::iterator m_end;
};

class DataLogReaderEntry : public wpi::log::StartRecordData {
 public:
  std::vector<DataLogReaderRange> ranges;  // ranges where this entry is valid
};

class DataLogReaderThread {
 public:
  explicit DataLogReaderThread(wpi::log::DataLogReader reader)
      : m_reader{std::move(reader)}, m_thread{[this] { ReadMain(); }} {}
  ~DataLogReaderThread();

  bool IsDone() const { return m_done; }
  std::string_view GetBufferIdentifier() const {
    return m_reader.GetBufferIdentifier();
  }
  unsigned int GetNumRecords() const { return m_numRecords; }
  unsigned int GetNumEntries() const {
    std::scoped_lock lock{m_mutex};
    return m_entriesByName.size();
  }

  // Passes Entry& to func
  template <typename T>
  void ForEachEntryName(T&& func) {
    std::scoped_lock lock{m_mutex};
    for (auto&& kv : m_entriesByName) {
      func(kv.second);
    }
  }

  const DataLogReaderEntry* GetEntry(std::string_view name) const {
    std::scoped_lock lock{m_mutex};
    auto it = m_entriesByName.find(name);
    if (it == m_entriesByName.end()) {
      return nullptr;
    }
    return &it->second;
  }

  wpi::StructDescriptorDatabase& GetStructDatabase() { return m_structDb; }
#ifndef NO_PROTOBUF
  wpi::ProtobufMessageDatabase& GetProtobufDatabase() { return m_protoDb; }
#endif

  const wpi::log::DataLogReader& GetReader() const { return m_reader; }

  // note: these are called on separate thread
  wpi::sig::Signal_mt<const DataLogReaderEntry&> sigEntryAdded;
  wpi::sig::Signal_mt<> sigDone;

 private:
  void ReadMain();

  wpi::log::DataLogReader m_reader;
  mutable wpi::mutex m_mutex;
  std::atomic_bool m_active{true};
  std::atomic_bool m_done{false};
  std::atomic<unsigned int> m_numRecords{0};
  std::map<std::string, DataLogReaderEntry, std::less<>> m_entriesByName;
  wpi::DenseMap<int, DataLogReaderEntry*> m_entriesById;
  wpi::StructDescriptorDatabase m_structDb;
#ifndef NO_PROTOBUF
  wpi::ProtobufMessageDatabase m_protoDb;
#endif
  std::thread m_thread;
};

}  // namespace glass

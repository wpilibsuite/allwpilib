// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/DataLog.h"

#include <algorithm>
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "wpi/Endian.h"
#include "wpi/Logger.h"
#include "wpi/SmallString.h"
#include "wpi/print.h"
#include "wpi/timestamp.h"

using namespace wpi::log;

static constexpr size_t kRecordMaxHeaderSize = 17;

static void DefaultLog(unsigned int level, const char* file, unsigned int line,
                       const char* msg) {
  if (level > wpi::WPI_LOG_INFO) {
    wpi::print(stderr, "DataLog: {}\n", msg);
  } else if (level == wpi::WPI_LOG_INFO) {
    wpi::print("DataLog: {}\n", msg);
  }
}

wpi::Logger DataLog::s_defaultMessageLog{DefaultLog};

template <typename T>
static unsigned int WriteVarInt(uint8_t* buf, T val) {
  unsigned int len = 0;
  do {
    *buf++ = static_cast<unsigned int>(val) & 0xff;
    ++len;
    val >>= 8;
  } while (val != 0);
  return len;
}

// min size: 4, max size: 17
static unsigned int WriteRecordHeader(uint8_t* buf, uint32_t entry,
                                      uint64_t timestamp,
                                      uint32_t payloadSize) {
  uint8_t* origbuf = buf++;

  unsigned int entryLen = WriteVarInt(buf, entry);
  buf += entryLen;
  unsigned int payloadLen = WriteVarInt(buf, payloadSize);
  buf += payloadLen;
  unsigned int timestampLen =
      WriteVarInt(buf, timestamp == 0 ? wpi::Now() : timestamp);
  buf += timestampLen;
  *origbuf =
      ((timestampLen - 1) << 4) | ((payloadLen - 1) << 2) | (entryLen - 1);
  return buf - origbuf;
}

void DataLog::StartFile() {
  std::scoped_lock lock{m_mutex};
  if (m_active) {
    return;
  }

  // Grab previously pending writes
  std::vector<Buffer> bufs;
  bufs.swap(m_outgoing);
  m_outgoing.reserve(bufs.size() + 1);

  // File header (version 1.0)
  uint8_t* buf = Reserve(m_extraHeader.size() + 12);
  static const uint8_t header[] = {'W', 'P', 'I', 'L', 'O', 'G', 0, 1};
  std::memcpy(buf, header, 8);
  support::endian::write32le(buf + 8, m_extraHeader.size());
  std::memcpy(buf + 12, m_extraHeader.data(), m_extraHeader.size());

  // Existing start and schema data records
  for (auto&& entryInfo : m_entries) {
    AppendStartRecord(entryInfo.second.id, entryInfo.first,
                      entryInfo.second.type,
                      m_entryIds[entryInfo.second.id].metadata, 0);
    if (!entryInfo.second.schemaData.empty()) {
      StartRecord(entryInfo.second.id, 0, entryInfo.second.schemaData.size(),
                  0);
      AppendImpl(entryInfo.second.schemaData);
    }
  }

  // Append previously pending writes
  for (auto&& buf : bufs) {
    m_outgoing.emplace_back(std::move(buf));
  }

  m_active = true;
}

void DataLog::FlushBufs(std::vector<Buffer>* writeBufs) {
  std::scoped_lock lock{m_mutex};
  writeBufs->swap(m_outgoing);
  DoReleaseBufs(&m_outgoing);
}

void DataLog::ReleaseBufs(std::vector<Buffer>* bufs) {
  std::scoped_lock lock{m_mutex};
  DoReleaseBufs(bufs);
}

void DataLog::Pause() {
  std::scoped_lock lock{m_mutex};
  m_paused = true;
}

void DataLog::Resume() {
  std::scoped_lock lock{m_mutex};
  m_paused = false;
}

void DataLog::Stop() {
  std::scoped_lock lock{m_mutex};
  m_active = false;
}

void DataLog::BufferHalfFull() {}

bool DataLog::HasSchema(std::string_view name) const {
  std::scoped_lock lock{m_mutex};
  wpi::SmallString<128> fullName{"/.schema/"};
  fullName += name;
  auto it = m_entries.find(fullName);
  return it != m_entries.end();
}

void DataLog::AddSchema(std::string_view name, std::string_view type,
                        std::span<const uint8_t> schema, int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  wpi::SmallString<128> fullName{"/.schema/"};
  fullName += name;
  auto& entryInfo = m_entries[fullName];
  if (entryInfo.id != 0) {
    return;  // don't add duplicates
  }
  entryInfo.schemaData.assign(schema.begin(), schema.end());
  int entry = StartImpl(fullName, type, {}, timestamp);

  // inline AppendRaw() without releasing lock
  if (entry <= 0) {
    [[unlikely]] return;  // should never happen, but check anyway
  }
  if (!m_active) {
    [[unlikely]] return;
  }
  StartRecord(entry, timestamp, schema.size(), 0);
  AppendImpl(schema);
}

// Control records use the following format:
// 1-byte type
// 4-byte entry
// rest of data (depending on type)

int DataLog::Start(std::string_view name, std::string_view type,
                   std::string_view metadata, int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  return StartImpl(name, type, metadata, timestamp);
}

int DataLog::StartImpl(std::string_view name, std::string_view type,
                       std::string_view metadata, int64_t timestamp) {
  auto& entryInfo = m_entries[name];
  if (entryInfo.id == 0) {
    entryInfo.id = ++m_lastId;
  }
  auto& entryInfo2 = m_entryIds[entryInfo.id];
  ++entryInfo2.count;
  if (entryInfo2.count > 1) {
    if (entryInfo.type != type) {
      WPI_ERROR(m_msglog,
                "type mismatch for '{}': was '{}', requested '{}'; ignoring",
                name, entryInfo.type, type);
      return 0;
    }
    return entryInfo.id;
  }
  entryInfo.type = type;
  entryInfo2.metadata = metadata;

  if (!m_active) {
    [[unlikely]] return entryInfo.id;
  }

  AppendStartRecord(entryInfo.id, name, type, metadata, timestamp);
  return entryInfo.id;
}

void DataLog::AppendStartRecord(int id, std::string_view name,
                                std::string_view type,
                                std::string_view metadata, int64_t timestamp) {
  size_t strsize = name.size() + type.size() + metadata.size();
  uint8_t* buf = StartRecord(0, timestamp, 5 + 12 + strsize, 5);
  *buf++ = impl::kControlStart;
  wpi::support::endian::write32le(buf, id);
  AppendStringImpl(name);
  AppendStringImpl(type);
  AppendStringImpl(metadata);
}

void DataLog::DoReleaseBufs(std::vector<Buffer>* bufs) {
  for (auto&& buf : *bufs) {
    buf.Clear();
    if (m_free.size() < kMaxFreeCount) {
      [[likely]] m_free.emplace_back(std::move(buf));
    }
  }
  bufs->resize(0);
}

void DataLog::Finish(int entry, int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  auto& entryInfo2 = m_entryIds[entry];
  if (entryInfo2.count == 0) {
    return;
  }
  --entryInfo2.count;
  if (entryInfo2.count != 0) {
    return;
  }
  m_entryIds.erase(entry);
  if (!m_active) {
    [[unlikely]] return;
  }
  uint8_t* buf = StartRecord(0, timestamp, 5, 5);
  *buf++ = impl::kControlFinish;
  wpi::support::endian::write32le(buf, entry);
}

void DataLog::SetMetadata(int entry, std::string_view metadata,
                          int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  m_entryIds[entry].metadata = metadata;
  if (!m_active) {
    [[unlikely]] return;
  }
  uint8_t* buf = StartRecord(0, timestamp, 5 + 4 + metadata.size(), 5);
  *buf++ = impl::kControlSetMetadata;
  wpi::support::endian::write32le(buf, entry);
  AppendStringImpl(metadata);
}

uint8_t* DataLog::Reserve(size_t size) {
  assert(size <= kBlockSize);
  if (m_outgoing.empty() || size > m_outgoing.back().GetRemaining()) {
    if (m_outgoing.size() == kMaxBufferCount / 2) {
      [[unlikely]] BufferHalfFull();
    }
    if (m_free.empty()) {
      if (m_outgoing.size() >= kMaxBufferCount) {
        [[unlikely]]
        if (BufferFull()) {
          m_paused = true;
        }
      }
      m_outgoing.emplace_back();
    } else {
      m_outgoing.emplace_back(std::move(m_free.back()));
      m_free.pop_back();
    }
  }
  return m_outgoing.back().Reserve(size);
}

uint8_t* DataLog::StartRecord(uint32_t entry, uint64_t timestamp,
                              uint32_t payloadSize, size_t reserveSize) {
  uint8_t* buf = Reserve(kRecordMaxHeaderSize + reserveSize);
  auto headerLen = WriteRecordHeader(buf, entry, timestamp, payloadSize);
  m_outgoing.back().Unreserve(kRecordMaxHeaderSize - headerLen);
  buf += headerLen;
  return buf;
}

void DataLog::AppendImpl(std::span<const uint8_t> data) {
  while (data.size() > kBlockSize) {
    uint8_t* buf = Reserve(kBlockSize);
    std::memcpy(buf, data.data(), kBlockSize);
    data = data.subspan(kBlockSize);
  }
  if (!data.empty()) {
    uint8_t* buf = Reserve(data.size());
    std::memcpy(buf, data.data(), data.size());
  }
}

void DataLog::AppendStringImpl(std::string_view str) {
  uint8_t* buf = Reserve(4);
  wpi::support::endian::write32le(buf, str.size());
  AppendImpl({reinterpret_cast<const uint8_t*>(str.data()), str.size()});
}

void DataLog::AppendRaw(int entry, std::span<const uint8_t> data,
                        int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  StartRecord(entry, timestamp, data.size(), 0);
  AppendImpl(data);
}

void DataLog::AppendRaw2(int entry,
                         std::span<const std::span<const uint8_t>> data,
                         int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  size_t size = 0;
  for (auto&& chunk : data) {
    size += chunk.size();
  }
  StartRecord(entry, timestamp, size, 0);
  for (auto chunk : data) {
    AppendImpl(chunk);
  }
}

void DataLog::AppendBoolean(int entry, bool value, int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  uint8_t* buf = StartRecord(entry, timestamp, 1, 1);
  buf[0] = value ? 1 : 0;
}

void DataLog::AppendInteger(int entry, int64_t value, int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  uint8_t* buf = StartRecord(entry, timestamp, 8, 8);
  wpi::support::endian::write64le(buf, value);
}

void DataLog::AppendFloat(int entry, float value, int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  uint8_t* buf = StartRecord(entry, timestamp, 4, 4);
  if constexpr (std::endian::native == std::endian::little) {
    std::memcpy(buf, &value, 4);
  } else {
    wpi::support::endian::write32le(buf, std::bit_cast<uint32_t>(value));
  }
}

void DataLog::AppendDouble(int entry, double value, int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  uint8_t* buf = StartRecord(entry, timestamp, 8, 8);
  if constexpr (std::endian::native == std::endian::little) {
    std::memcpy(buf, &value, 8);
  } else {
    wpi::support::endian::write64le(buf, std::bit_cast<uint64_t>(value));
  }
}

void DataLog::AppendString(int entry, std::string_view value,
                           int64_t timestamp) {
  AppendRaw(entry,
            {reinterpret_cast<const uint8_t*>(value.data()), value.size()},
            timestamp);
}

void DataLog::AppendBooleanArray(int entry, std::span<const bool> arr,
                                 int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  StartRecord(entry, timestamp, arr.size(), 0);
  uint8_t* buf;
  while (arr.size() > kBlockSize) {
    buf = Reserve(kBlockSize);
    for (auto val : arr.subspan(0, kBlockSize)) {
      *buf++ = val ? 1 : 0;
    }
    arr = arr.subspan(kBlockSize);
  }
  buf = Reserve(arr.size());
  for (auto val : arr) {
    *buf++ = val ? 1 : 0;
  }
}

void DataLog::AppendBooleanArray(int entry, std::span<const int> arr,
                                 int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  StartRecord(entry, timestamp, arr.size(), 0);
  uint8_t* buf;
  while (arr.size() > kBlockSize) {
    buf = Reserve(kBlockSize);
    for (auto val : arr.subspan(0, kBlockSize)) {
      *buf++ = val & 1;
    }
    arr = arr.subspan(kBlockSize);
  }
  buf = Reserve(arr.size());
  for (auto val : arr) {
    *buf++ = val & 1;
  }
}

void DataLog::AppendBooleanArray(int entry, std::span<const uint8_t> arr,
                                 int64_t timestamp) {
  AppendRaw(entry, arr, timestamp);
}

void DataLog::AppendIntegerArray(int entry, std::span<const int64_t> arr,
                                 int64_t timestamp) {
  if constexpr (std::endian::native == std::endian::little) {
    AppendRaw(entry,
              {reinterpret_cast<const uint8_t*>(arr.data()), arr.size() * 8},
              timestamp);
  } else {
    if (entry <= 0) {
      return;
    }
    std::scoped_lock lock{m_mutex};
    if (m_paused) {
      [[unlikely]] return;
    }
    StartRecord(entry, timestamp, arr.size() * 8, 0);
    uint8_t* buf;
    while ((arr.size() * 8) > kBlockSize) {
      buf = Reserve(kBlockSize);
      for (auto val : arr.subspan(0, kBlockSize / 8)) {
        wpi::support::endian::write64le(buf, val);
        buf += 8;
      }
      arr = arr.subspan(kBlockSize / 8);
    }
    buf = Reserve(arr.size() * 8);
    for (auto val : arr) {
      wpi::support::endian::write64le(buf, val);
      buf += 8;
    }
  }
}

void DataLog::AppendFloatArray(int entry, std::span<const float> arr,
                               int64_t timestamp) {
  if constexpr (std::endian::native == std::endian::little) {
    AppendRaw(entry,
              {reinterpret_cast<const uint8_t*>(arr.data()), arr.size() * 4},
              timestamp);
  } else {
    if (entry <= 0) {
      return;
    }
    std::scoped_lock lock{m_mutex};
    if (m_paused) {
      [[unlikely]] return;
    }
    StartRecord(entry, timestamp, arr.size() * 4, 0);
    uint8_t* buf;
    while ((arr.size() * 4) > kBlockSize) {
      buf = Reserve(kBlockSize);
      for (auto val : arr.subspan(0, kBlockSize / 4)) {
        wpi::support::endian::write32le(buf, std::bit_cast<uint32_t>(val));
        buf += 4;
      }
      arr = arr.subspan(kBlockSize / 4);
    }
    buf = Reserve(arr.size() * 4);
    for (auto val : arr) {
      wpi::support::endian::write32le(buf, std::bit_cast<uint32_t>(val));
      buf += 4;
    }
  }
}

void DataLog::AppendDoubleArray(int entry, std::span<const double> arr,
                                int64_t timestamp) {
  if constexpr (std::endian::native == std::endian::little) {
    AppendRaw(entry,
              {reinterpret_cast<const uint8_t*>(arr.data()), arr.size() * 8},
              timestamp);
  } else {
    if (entry <= 0) {
      return;
    }
    std::scoped_lock lock{m_mutex};
    if (m_paused) {
      [[unlikely]] return;
    }
    StartRecord(entry, timestamp, arr.size() * 8, 0);
    uint8_t* buf;
    while ((arr.size() * 8) > kBlockSize) {
      buf = Reserve(kBlockSize);
      for (auto val : arr.subspan(0, kBlockSize / 8)) {
        wpi::support::endian::write64le(buf, std::bit_cast<uint64_t>(val));
        buf += 8;
      }
      arr = arr.subspan(kBlockSize / 8);
    }
    buf = Reserve(arr.size() * 8);
    for (auto val : arr) {
      wpi::support::endian::write64le(buf, std::bit_cast<uint64_t>(val));
      buf += 8;
    }
  }
}

void DataLog::AppendStringArray(int entry, std::span<const std::string> arr,
                                int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  // storage: 4-byte array length, each string prefixed by 4-byte length
  // calculate total size
  size_t size = 4;
  for (auto&& str : arr) {
    size += 4 + str.size();
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  uint8_t* buf = StartRecord(entry, timestamp, size, 4);
  wpi::support::endian::write32le(buf, arr.size());
  for (auto&& str : arr) {
    AppendStringImpl(str);
  }
}

void DataLog::AppendStringArray(int entry,
                                std::span<const std::string_view> arr,
                                int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  // storage: 4-byte array length, each string prefixed by 4-byte length
  // calculate total size
  size_t size = 4;
  for (auto&& str : arr) {
    size += 4 + str.size();
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  uint8_t* buf = StartRecord(entry, timestamp, size, 4);
  wpi::support::endian::write32le(buf, arr.size());
  for (auto&& sv : arr) {
    AppendStringImpl(sv);
  }
}

void DataLog::AppendStringArray(int entry,
                                std::span<const struct WPI_String> arr,
                                int64_t timestamp) {
  if (entry <= 0) {
    return;
  }
  // storage: 4-byte array length, each string prefixed by 4-byte length
  // calculate total size
  size_t size = 4;
  for (auto&& str : arr) {
    size += 4 + str.len;
  }
  std::scoped_lock lock{m_mutex};
  if (m_paused) {
    [[unlikely]] return;
  }
  uint8_t* buf = StartRecord(entry, timestamp, size, 4);
  wpi::support::endian::write32le(buf, arr.size());
  for (auto&& sv : arr) {
    AppendStringImpl(sv.str);
  }
}

template <typename V1, typename V2>
inline bool UpdateImpl(std::optional<std::vector<V1>>& lastValue,
                       std::span<const V2> data) {
  if (!lastValue || !std::equal(data.begin(), data.end(), lastValue->begin(),
                                lastValue->end())) {
    if (lastValue) {
      lastValue->assign(data.begin(), data.end());
    } else {
      lastValue = std::vector<V1>{data.begin(), data.end()};
    }
    return true;
  }
  return false;
}

template <typename V1>
inline bool UpdateImpl(std::optional<std::vector<V1>>& lastValue,
                       std::span<const bool> data) {
  if (!lastValue || !std::equal(data.begin(), data.end(), lastValue->begin(),
                                lastValue->end(), [](auto a, auto b) {
                                  return a == static_cast<bool>(b);
                                })) {
    if (lastValue) {
      lastValue->assign(data.begin(), data.end());
    } else {
      lastValue = std::vector<V1>{data.begin(), data.end()};
    }
    return true;
  }
  return false;
}

void RawLogEntry::Update(std::span<const uint8_t> data, int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  if (UpdateImpl(m_lastValue, data)) {
    Append(data, timestamp);
  }
}

void BooleanArrayLogEntry::Update(std::span<const bool> arr,
                                  int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  if (UpdateImpl(m_lastValue, arr)) {
    Append(arr, timestamp);
  }
}

void BooleanArrayLogEntry::Update(std::span<const int> arr, int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  if (UpdateImpl(m_lastValue, arr)) {
    Append(arr, timestamp);
  }
}

void BooleanArrayLogEntry::Update(std::span<const uint8_t> arr,
                                  int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  if (UpdateImpl(m_lastValue, arr)) {
    Append(arr, timestamp);
  }
}

void IntegerArrayLogEntry::Update(std::span<const int64_t> arr,
                                  int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  if (UpdateImpl(m_lastValue, arr)) {
    Append(arr, timestamp);
  }
}

void FloatArrayLogEntry::Update(std::span<const float> arr, int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  if (UpdateImpl(m_lastValue, arr)) {
    Append(arr, timestamp);
  }
}

void DoubleArrayLogEntry::Update(std::span<const double> arr,
                                 int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  if (UpdateImpl(m_lastValue, arr)) {
    Append(arr, timestamp);
  }
}

void StringArrayLogEntry::Update(std::span<const std::string> arr,
                                 int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  if (UpdateImpl(m_lastValue, arr)) {
    Append(arr, timestamp);
  }
}

void StringArrayLogEntry::Update(std::span<const std::string_view> arr,
                                 int64_t timestamp) {
  std::scoped_lock lock{m_mutex};
  if (UpdateImpl(m_lastValue, arr)) {
    Append(arr, timestamp);
  }
}

extern "C" {

void WPI_DataLog_Release(struct WPI_DataLog* datalog) {
  delete reinterpret_cast<DataLog*>(datalog);
}

void WPI_DataLog_Flush(struct WPI_DataLog* datalog) {
  reinterpret_cast<DataLog*>(datalog)->Flush();
}

void WPI_DataLog_Pause(struct WPI_DataLog* datalog) {
  reinterpret_cast<DataLog*>(datalog)->Pause();
}

void WPI_DataLog_Resume(struct WPI_DataLog* datalog) {
  reinterpret_cast<DataLog*>(datalog)->Resume();
}

void WPI_DataLog_Stop(struct WPI_DataLog* datalog) {
  reinterpret_cast<DataLog*>(datalog)->Stop();
}

int WPI_DataLog_Start(struct WPI_DataLog* datalog,
                      const struct WPI_String* name,
                      const struct WPI_String* type,
                      const struct WPI_String* metadata, int64_t timestamp) {
  return reinterpret_cast<DataLog*>(datalog)->Start(
      wpi::to_string_view(name), wpi::to_string_view(type),
      wpi::to_string_view(metadata), timestamp);
}

void WPI_DataLog_Finish(struct WPI_DataLog* datalog, int entry,
                        int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->Finish(entry, timestamp);
}

void WPI_DataLog_SetMetadata(struct WPI_DataLog* datalog, int entry,
                             const struct WPI_String* metadata,
                             int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->SetMetadata(
      entry, wpi::to_string_view(metadata), timestamp);
}

void WPI_DataLog_AppendRaw(struct WPI_DataLog* datalog, int entry,
                           const uint8_t* data, size_t len, int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendRaw(entry, {data, len}, timestamp);
}

void WPI_DataLog_AppendBoolean(struct WPI_DataLog* datalog, int entry,
                               int value, int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendBoolean(entry, value, timestamp);
}

void WPI_DataLog_AppendInteger(struct WPI_DataLog* datalog, int entry,
                               int64_t value, int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendInteger(entry, value, timestamp);
}

void WPI_DataLog_AppendFloat(struct WPI_DataLog* datalog, int entry,
                             float value, int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendFloat(entry, value, timestamp);
}

void WPI_DataLog_AppendDouble(struct WPI_DataLog* datalog, int entry,
                              double value, int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendDouble(entry, value, timestamp);
}

void WPI_DataLog_AppendString(struct WPI_DataLog* datalog, int entry,
                              const struct WPI_String* value,
                              int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendString(
      entry, {value->str, value->len}, timestamp);
}

void WPI_DataLog_AppendBooleanArray(struct WPI_DataLog* datalog, int entry,
                                    const int* arr, size_t len,
                                    int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendBooleanArray(entry, {arr, len},
                                                          timestamp);
}

void WPI_DataLog_AppendBooleanArrayByte(struct WPI_DataLog* datalog, int entry,
                                        const uint8_t* arr, size_t len,
                                        int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendBooleanArray(entry, {arr, len},
                                                          timestamp);
}

void WPI_DataLog_AppendIntegerArray(struct WPI_DataLog* datalog, int entry,
                                    const int64_t* arr, size_t len,
                                    int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendIntegerArray(entry, {arr, len},
                                                          timestamp);
}

void WPI_DataLog_AppendFloatArray(struct WPI_DataLog* datalog, int entry,
                                  const float* arr, size_t len,
                                  int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendFloatArray(entry, {arr, len},
                                                        timestamp);
}

void WPI_DataLog_AppendDoubleArray(struct WPI_DataLog* datalog, int entry,
                                   const double* arr, size_t len,
                                   int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendDoubleArray(entry, {arr, len},
                                                         timestamp);
}

void WPI_DataLog_AppendStringArray(struct WPI_DataLog* datalog, int entry,
                                   const struct WPI_String* arr, size_t len,
                                   int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AppendStringArray(entry, {arr, len},
                                                         timestamp);
}

void WPI_DataLog_AddSchemaString(struct WPI_DataLog* datalog,
                                 const struct WPI_String* name,
                                 const struct WPI_String* type,
                                 const struct WPI_String* schema,
                                 int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AddSchema(
      wpi::to_string_view(name), wpi::to_string_view(type),
      wpi::to_string_view(schema), timestamp);
}

void WPI_DataLog_AddSchema(struct WPI_DataLog* datalog,
                           const struct WPI_String* name,
                           const struct WPI_String* type, const uint8_t* schema,
                           size_t schema_len, int64_t timestamp) {
  reinterpret_cast<DataLog*>(datalog)->AddSchema(
      wpi::to_string_view(name), wpi::to_string_view(type),
      std::span<const uint8_t>{schema, schema_len}, timestamp);
}

}  // extern "C"

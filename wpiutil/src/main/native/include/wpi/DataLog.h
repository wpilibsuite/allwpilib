// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
#include <version>

#include "wpi/DataLog_c.h"
#include "wpi/DenseMap.h"
#include "wpi/SmallVector.h"
#include "wpi/StringMap.h"
#include "wpi/mutex.h"
#include "wpi/protobuf/Protobuf.h"
#include "wpi/string.h"
#include "wpi/struct/Struct.h"
#include "wpi/timestamp.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace wpi::log {

namespace impl {

enum ControlRecordType {
  kControlStart = 0,
  kControlFinish,
  kControlSetMetadata
};

}  // namespace impl

/**
 * A data log for high-speed writing of data values.
 *
 * The lifetime of the data log object must be longer than any data log entry
 * objects that refer to it.
 *
 * Finish() is needed only to indicate in the log that a particular entry is
 * no longer being used (it releases the name to ID mapping).  Finish() is not
 * required to be called for data to be flushed to disk; entries in the log
 * are written as Append() calls are being made.  In fact, Finish() does not
 * need to be called at all; this is helpful to avoid shutdown races where the
 * DataLog object might be destroyed before other objects.  It's often not a
 * good idea to call Finish() from destructors for this reason.
 *
 * DataLog calls are thread safe.  DataLog uses a typical multiple-supplier,
 * single-consumer setup.  Writes to the log are atomic, but there is no
 * guaranteed order in the log when multiple threads are writing to it;
 * whichever thread grabs the write mutex first will get written first.
 * For this reason (as well as the fact that timestamps can be set to
 * arbitrary values), records in the log are not guaranteed to be sorted by
 * timestamp.
 */
class DataLog {
 public:
  virtual ~DataLog() = default;

  DataLog(const DataLog&) = delete;
  DataLog& operator=(const DataLog&) = delete;
  DataLog(DataLog&&) = delete;
  DataLog& operator=(const DataLog&&) = delete;

  /**
   * Explicitly flushes the log data to disk.
   */
  virtual void Flush() = 0;

  /**
   * Pauses appending of data records to the log.  While paused, no data records
   * are saved (e.g. AppendX is a no-op).  Has no effect on entry starts /
   * finishes / metadata changes.
   */
  virtual void Pause();

  /**
   * Resumes appending of data records to the log.
   */
  virtual void Resume();

  /**
   * Stops appending start/metadata/schema records to the log.
   */
  virtual void Stop();

  /**
   * Returns whether there is a data schema already registered with the given
   * name.
   *
   * @param name Name (the string passed as the data type for records using this
   *             schema)
   * @return True if schema already registered
   */
  bool HasSchema(std::string_view name) const;

  /**
   * Registers a data schema.  Data schemas provide information for how a
   * certain data type string can be decoded.  The type string of a data schema
   * indicates the type of the schema itself (e.g. "protobuf" for protobuf
   * schemas, "struct" for struct schemas, etc). In the data log, schemas are
   * saved just like normal records, with the name being generated from the
   * provided name: "/.schema/<name>".  Duplicate calls to this function with
   * the same name are silently ignored.
   *
   * @param name Name (the string passed as the data type for records using this
   *             schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AddSchema(std::string_view name, std::string_view type,
                 std::span<const uint8_t> schema, int64_t timestamp = 0);

  /**
   * Registers a data schema.  Data schemas provide information for how a
   * certain data type string can be decoded.  The type string of a data schema
   * indicates the type of the schema itself (e.g. "protobuf" for protobuf
   * schemas, "struct" for struct schemas, etc). In the data log, schemas are
   * saved just like normal records, with the name being generated from the
   * provided name: "/.schema/<name>".  Duplicate calls to this function with
   * the same name are silently ignored.
   *
   * @param name Name (the string passed as the data type for records using this
   *             schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AddSchema(std::string_view name, std::string_view type,
                 std::string_view schema, int64_t timestamp = 0) {
    AddSchema(
        name, type,
        std::span<const uint8_t>{
            reinterpret_cast<const uint8_t*>(schema.data()), schema.size()},
        timestamp);
  }

  /**
   * Registers a protobuf schema. Duplicate calls to this function with the same
   * name are silently ignored.
   *
   * @tparam T protobuf serializable type
   * @param msg protobuf message
   * @param timestamp Time stamp (0 to indicate now)
   */
  template <ProtobufSerializable T>
  void AddProtobufSchema(ProtobufMessage<T>& msg, int64_t timestamp = 0) {
    if (timestamp == 0) {
      timestamp = Now();
    }
    msg.ForEachProtobufDescriptor(
        [this](auto typeString) { return HasSchema(typeString); },
        [this, timestamp](auto typeString, auto schema) {
          AddSchema(typeString, "proto:FileDescriptorProto", schema, timestamp);
        });
  }

  /**
   * Registers a struct schema. Duplicate calls to this function with the same
   * name are silently ignored.
   *
   * @tparam T struct serializable type
   * @param info optional struct type info
   * @param timestamp Time stamp (0 to indicate now)
   */
  template <typename T, typename... I>
    requires StructSerializable<T, I...>
  void AddStructSchema(const I&... info, int64_t timestamp = 0) {
    if (timestamp == 0) {
      timestamp = Now();
    }
    ForEachStructSchema<T>(
        [this, timestamp](auto typeString, auto schema) {
          this->AddSchema(typeString, "structschema", schema, timestamp);
        },
        info...);
  }

  /**
   * Start an entry.  Duplicate names are allowed (with the same type), and
   * result in the same index being returned (Start/Finish are reference
   * counted).  A duplicate name with a different type will result in an error
   * message being printed to the console and 0 being returned (which will be
   * ignored by the Append functions).
   *
   * @param name Name
   * @param type Data type
   * @param metadata Initial metadata (e.g. data properties)
   * @param timestamp Time stamp (may be 0 to indicate now)
   *
   * @return Entry index
   */
  int Start(std::string_view name, std::string_view type,
            std::string_view metadata = {}, int64_t timestamp = 0);

  /**
   * Finish an entry.
   *
   * @param entry Entry index
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Finish(int entry, int64_t timestamp = 0);

  /**
   * Updates the metadata for an entry.
   *
   * @param entry Entry index
   * @param metadata New metadata for the entry
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void SetMetadata(int entry, std::string_view metadata, int64_t timestamp = 0);

  /**
   * Appends a raw record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param data Byte array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendRaw(int entry, std::span<const uint8_t> data, int64_t timestamp);

  /**
   * Appends a raw record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param data Byte array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendRaw2(int entry, std::span<const std::span<const uint8_t>> data,
                  int64_t timestamp);

  /**
   * Appends a boolean record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param value Boolean value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendBoolean(int entry, bool value, int64_t timestamp);

  /**
   * Appends an integer record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param value Integer value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendInteger(int entry, int64_t value, int64_t timestamp);

  /**
   * Appends a float record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param value Float value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendFloat(int entry, float value, int64_t timestamp);

  /**
   * Appends a double record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param value Double value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendDouble(int entry, double value, int64_t timestamp);

  /**
   * Appends a string record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param value String value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendString(int entry, std::string_view value, int64_t timestamp);

  /**
   * Appends a boolean array record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param arr Boolean array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendBooleanArray(int entry, std::span<const bool> arr,
                          int64_t timestamp);

  /**
   * Appends a boolean array record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param arr Boolean array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendBooleanArray(int entry, std::span<const int> arr,
                          int64_t timestamp);

  /**
   * Appends a boolean array record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param arr Boolean array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendBooleanArray(int entry, std::span<const uint8_t> arr,
                          int64_t timestamp);

  /**
   * Appends an integer array record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param arr Integer array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendIntegerArray(int entry, std::span<const int64_t> arr,
                          int64_t timestamp);

  /**
   * Appends a float array record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param arr Float array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendFloatArray(int entry, std::span<const float> arr,
                        int64_t timestamp);

  /**
   * Appends a double array record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param arr Double array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendDoubleArray(int entry, std::span<const double> arr,
                         int64_t timestamp);

  /**
   * Appends a string array record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param arr String array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendStringArray(int entry, std::span<const std::string> arr,
                         int64_t timestamp);

  /**
   * Appends a string array record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param arr String array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendStringArray(int entry, std::span<const std::string_view> arr,
                         int64_t timestamp);

  /**
   * Appends a string array record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param arr String array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendStringArray(int entry, std::span<const struct WPI_String> arr,
                         int64_t timestamp);

 protected:
  static constexpr size_t kBlockSize = 16 * 1024;
  static wpi::Logger s_defaultMessageLog;

  class Buffer {
   public:
    explicit Buffer(size_t alloc = kBlockSize)
        : m_buf{new uint8_t[alloc]}, m_maxLen{alloc} {}
    ~Buffer() { delete[] m_buf; }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& oth)
        : m_buf{oth.m_buf}, m_len{oth.m_len}, m_maxLen{oth.m_maxLen} {
      oth.m_buf = nullptr;
      oth.m_len = 0;
      oth.m_maxLen = 0;
    }

    Buffer& operator=(Buffer&& oth) {
      if (m_buf) {
        delete[] m_buf;
      }
      m_buf = oth.m_buf;
      m_len = oth.m_len;
      m_maxLen = oth.m_maxLen;
      oth.m_buf = nullptr;
      oth.m_len = 0;
      oth.m_maxLen = 0;
      return *this;
    }

    uint8_t* Reserve(size_t size) {
      assert(size <= GetRemaining());
      uint8_t* rv = m_buf + m_len;
      m_len += size;
      return rv;
    }

    void Unreserve(size_t size) { m_len -= size; }

    void Clear() { m_len = 0; }

    size_t GetRemaining() const { return m_maxLen - m_len; }

    std::span<uint8_t> GetData() { return {m_buf, m_len}; }
    std::span<const uint8_t> GetData() const { return {m_buf, m_len}; }

   private:
    uint8_t* m_buf;
    size_t m_len = 0;
    size_t m_maxLen;
  };

  /**
   * Constructs the log.  StartFile() must be called to actually start the
   * file output.
   *
   * @param msglog message logger (will be called from separate thread)
   * @param extraHeader extra header metadata
   */
  explicit DataLog(wpi::Logger& msglog, std::string_view extraHeader = "")
      : m_msglog{msglog}, m_extraHeader{extraHeader} {}

  /**
   * Starts the log.  Appends file header and Start records and schema data
   * values for all previously started entries and schemas.  No effect unless
   * the data log is currently stopped.
   */
  void StartFile();

  /**
   * Provides complete set of all buffers that need to be written.
   *
   * Any existing contents of writeBufs will be released as if ReleaseBufs()
   * was called prior to this call.
   *
   * Returned buffers should provided back via ReleaseBufs() after the write is
   * complete.
   *
   * @param writeBufs buffers to be written (output)
   */
  void FlushBufs(std::vector<Buffer>* writeBufs);

  /**
   * Releases memory for a set of buffers back to the internal buffer pool.
   *
   * @param bufs buffers; empty on return
   */
  void ReleaseBufs(std::vector<Buffer>* bufs);

  /**
   * Called when internal buffers are half the maximum count.  Called with
   * internal mutex held; do not call any other DataLog functions from this
   * function.
   */
  virtual void BufferHalfFull();

  /**
   * Called when internal buffers reach the maximum count.  Called with internal
   * mutex held; do not call any other DataLog functions from this function.
   *
   * @return true if log should be paused (don't call PauseLog)
   */
  virtual bool BufferFull() = 0;

 private:
  static constexpr size_t kMaxBufferCount = 1024 * 1024 / kBlockSize;
  static constexpr size_t kMaxFreeCount = 256 * 1024 / kBlockSize;

  // must be called with m_mutex held
  int StartImpl(std::string_view name, std::string_view type,
                std::string_view metadata, int64_t timestamp);
  uint8_t* StartRecord(uint32_t entry, uint64_t timestamp, uint32_t payloadSize,
                       size_t reserveSize);
  uint8_t* Reserve(size_t size);
  void AppendImpl(std::span<const uint8_t> data);
  void AppendStringImpl(std::string_view str);
  void AppendStartRecord(int id, std::string_view name, std::string_view type,
                         std::string_view metadata, int64_t timestamp);
  void DoReleaseBufs(std::vector<Buffer>* bufs);

 protected:
  wpi::Logger& m_msglog;

 private:
  mutable wpi::mutex m_mutex;
  bool m_active = false;
  bool m_paused = false;
  std::string m_extraHeader;
  std::vector<Buffer> m_free;
  std::vector<Buffer> m_outgoing;
  struct EntryInfo {
    std::string type;
    std::vector<uint8_t> schemaData;  // only set for schema entries
    int id{0};
  };
  wpi::StringMap<EntryInfo> m_entries;
  struct EntryInfo2 {
    std::string metadata;
    unsigned int count;
  };
  wpi::DenseMap<int, EntryInfo2> m_entryIds;
  int m_lastId = 0;
};

/**
 * Log entry base class.
 */
class DataLogEntry {
 protected:
  DataLogEntry() = default;
  DataLogEntry(DataLog& log, std::string_view name, std::string_view type,
               std::string_view metadata = {}, int64_t timestamp = 0)
      : m_log{&log}, m_entry{log.Start(name, type, metadata, timestamp)} {}

 public:
  DataLogEntry(const DataLogEntry&) = delete;
  DataLogEntry& operator=(const DataLogEntry&) = delete;

  DataLogEntry(DataLogEntry&& rhs) : m_log{rhs.m_log}, m_entry{rhs.m_entry} {
    rhs.m_log = nullptr;
  }
  DataLogEntry& operator=(DataLogEntry&& rhs) {
    if (m_log) {
      m_log->Finish(m_entry);
    }
    m_log = rhs.m_log;
    rhs.m_log = nullptr;
    m_entry = rhs.m_entry;
    return *this;
  }

  explicit operator bool() const { return m_log != nullptr; }

  /**
   * Updates the metadata for the entry.
   *
   * @param metadata New metadata for the entry
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void SetMetadata(std::string_view metadata, int64_t timestamp = 0) {
    m_log->SetMetadata(m_entry, metadata, timestamp);
  }

  /**
   * Finishes the entry.
   *
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Finish(int64_t timestamp = 0) { m_log->Finish(m_entry, timestamp); }

 protected:
  DataLog* m_log = nullptr;
  int m_entry = 0;
};

template <typename T>
class DataLogValueEntryImpl : public DataLogEntry {
 protected:
  DataLogValueEntryImpl() = default;
  DataLogValueEntryImpl(DataLog& log, std::string_view name,
                        std::string_view type, std::string_view metadata = {},
                        int64_t timestamp = 0)
      : DataLogEntry{log, name, type, metadata, timestamp} {}

 public:
  DataLogValueEntryImpl(DataLogValueEntryImpl&& rhs)
      : DataLogEntry{std::move(rhs)} {
    std::scoped_lock lock{rhs.m_mutex};
    m_lastValue = std::move(rhs.m_lastValue);
  }
  DataLogValueEntryImpl& operator=(DataLogValueEntryImpl&& rhs) {
    DataLogEntry::operator=(std::move(rhs));
    std::scoped_lock lock{m_mutex, rhs.m_mutex};
    m_lastValue = std::move(rhs.m_lastValue);
    return *this;
  }

  /**
   * Gets whether there is a last value.
   *
   * @note The last value is local to this class instance and updated only with
   * Update(), not Append().
   *
   * @return True if last value exists, false otherwise.
   */
  bool HasLastValue() const { return m_lastValue.has_value(); }

  /**
   * Gets the last value.
   *
   * @note The last value is local to this class instance and updated only with
   * Update(), not Append().
   *
   * @return Last value (empty if no last value)
   */
  std::optional<T> GetLastValue() const {
    std::scoped_lock lock{m_mutex};
    return m_lastValue;
  }

 protected:
  mutable wpi::mutex m_mutex;
  std::optional<T> m_lastValue;
};

/**
 * Log arbitrary byte data.
 */
class RawLogEntry : public DataLogValueEntryImpl<std::vector<uint8_t>> {
 public:
  static constexpr std::string_view kDataType = "raw";

  RawLogEntry() = default;
  RawLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : RawLogEntry{log, name, {}, kDataType, timestamp} {}
  RawLogEntry(DataLog& log, std::string_view name, std::string_view metadata,
              int64_t timestamp = 0)
      : RawLogEntry{log, name, metadata, kDataType, timestamp} {}
  RawLogEntry(DataLog& log, std::string_view name, std::string_view metadata,
              std::string_view type, int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, type, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const uint8_t> data, int64_t timestamp = 0) {
    m_log->AppendRaw(m_entry, data, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::span<const uint8_t> data, int64_t timestamp = 0);
};

/**
 * Log boolean values.
 */
class BooleanLogEntry : public DataLogValueEntryImpl<bool> {
 public:
  static constexpr std::string_view kDataType = "boolean";

  BooleanLogEntry() = default;
  BooleanLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : BooleanLogEntry{log, name, {}, timestamp} {}
  BooleanLogEntry(DataLog& log, std::string_view name,
                  std::string_view metadata, int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(bool value, int64_t timestamp = 0) {
    m_log->AppendBoolean(m_entry, value, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(bool value, int64_t timestamp = 0) {
    std::scoped_lock lock{m_mutex};
    if (m_lastValue != value) {
      m_lastValue = value;
      Append(value, timestamp);
    }
  }
};

/**
 * Log integer values.
 */
class IntegerLogEntry : public DataLogValueEntryImpl<int64_t> {
 public:
  static constexpr std::string_view kDataType = "int64";

  IntegerLogEntry() = default;
  IntegerLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : IntegerLogEntry{log, name, {}, timestamp} {}
  IntegerLogEntry(DataLog& log, std::string_view name,
                  std::string_view metadata, int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(int64_t value, int64_t timestamp = 0) {
    m_log->AppendInteger(m_entry, value, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(int64_t value, int64_t timestamp = 0) {
    std::scoped_lock lock{m_mutex};
    if (m_lastValue != value) {
      m_lastValue = value;
      Append(value, timestamp);
    }
  }
};

/**
 * Log float values.
 */
class FloatLogEntry : public DataLogValueEntryImpl<float> {
 public:
  static constexpr std::string_view kDataType = "float";

  FloatLogEntry() = default;
  FloatLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : FloatLogEntry{log, name, {}, timestamp} {}
  FloatLogEntry(DataLog& log, std::string_view name, std::string_view metadata,
                int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(float value, int64_t timestamp = 0) {
    m_log->AppendFloat(m_entry, value, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(float value, int64_t timestamp = 0) {
    std::scoped_lock lock{m_mutex};
    if (m_lastValue != value) {
      m_lastValue = value;
      Append(value, timestamp);
    }
  }
};

/**
 * Log double values.
 */
class DoubleLogEntry : public DataLogValueEntryImpl<double> {
 public:
  static constexpr std::string_view kDataType = "double";

  DoubleLogEntry() = default;
  DoubleLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : DoubleLogEntry{log, name, {}, timestamp} {}
  DoubleLogEntry(DataLog& log, std::string_view name, std::string_view metadata,
                 int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(double value, int64_t timestamp = 0) {
    m_log->AppendDouble(m_entry, value, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(double value, int64_t timestamp = 0) {
    std::scoped_lock lock{m_mutex};
    if (m_lastValue != value) {
      m_lastValue = value;
      Append(value, timestamp);
    }
  }
};

/**
 * Log string values.
 */
class StringLogEntry : public DataLogValueEntryImpl<std::string> {
 public:
  static constexpr const char* kDataType = "string";

  StringLogEntry() = default;
  StringLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : StringLogEntry{log, name, {}, kDataType, timestamp} {}
  StringLogEntry(DataLog& log, std::string_view name, std::string_view metadata,
                 int64_t timestamp = 0)
      : StringLogEntry{log, name, metadata, kDataType, timestamp} {}
  StringLogEntry(DataLog& log, std::string_view name, std::string_view metadata,
                 std::string_view type, int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, type, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::string_view value, int64_t timestamp = 0) {
    m_log->AppendString(m_entry, value, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::string_view value, int64_t timestamp = 0) {
    std::scoped_lock lock{m_mutex};
    if (m_lastValue != value) {
      m_lastValue = value;
      Append(value, timestamp);
    }
  }
};

/**
 * Log array of boolean values.
 */
class BooleanArrayLogEntry : public DataLogValueEntryImpl<std::vector<int>> {
 public:
  static constexpr const char* kDataType = "boolean[]";

  BooleanArrayLogEntry() = default;
  BooleanArrayLogEntry(DataLog& log, std::string_view name,
                       int64_t timestamp = 0)
      : BooleanArrayLogEntry{log, name, {}, timestamp} {}
  BooleanArrayLogEntry(DataLog& log, std::string_view name,
                       std::string_view metadata, int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const bool> arr, int64_t timestamp = 0) {
    m_log->AppendBooleanArray(m_entry, arr, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::initializer_list<bool> arr, int64_t timestamp = 0) {
    Append(std::span{arr.begin(), arr.end()}, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const int> arr, int64_t timestamp = 0) {
    m_log->AppendBooleanArray(m_entry, arr, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::initializer_list<int> arr, int64_t timestamp = 0) {
    Append(std::span{arr.begin(), arr.end()}, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const uint8_t> arr, int64_t timestamp = 0) {
    m_log->AppendBooleanArray(m_entry, arr, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::span<const bool> arr, int64_t timestamp = 0);

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::initializer_list<bool> arr, int64_t timestamp = 0) {
    Update(std::span{arr.begin(), arr.end()}, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::span<const int> arr, int64_t timestamp = 0);

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::initializer_list<int> arr, int64_t timestamp = 0) {
    Update(std::span{arr.begin(), arr.end()}, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::span<const uint8_t> arr, int64_t timestamp = 0);
};

/**
 * Log array of integer values.
 */
class IntegerArrayLogEntry
    : public DataLogValueEntryImpl<std::vector<int64_t>> {
 public:
  static constexpr const char* kDataType = "int64[]";

  IntegerArrayLogEntry() = default;
  IntegerArrayLogEntry(DataLog& log, std::string_view name,
                       int64_t timestamp = 0)
      : IntegerArrayLogEntry{log, name, {}, timestamp} {}
  IntegerArrayLogEntry(DataLog& log, std::string_view name,
                       std::string_view metadata, int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const int64_t> arr, int64_t timestamp = 0) {
    m_log->AppendIntegerArray(m_entry, arr, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::initializer_list<int64_t> arr, int64_t timestamp = 0) {
    Append({arr.begin(), arr.end()}, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::span<const int64_t> arr, int64_t timestamp = 0);

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::initializer_list<int64_t> arr, int64_t timestamp = 0) {
    Update({arr.begin(), arr.end()}, timestamp);
  }
};

/**
 * Log array of float values.
 */
class FloatArrayLogEntry : public DataLogValueEntryImpl<std::vector<float>> {
 public:
  static constexpr const char* kDataType = "float[]";

  FloatArrayLogEntry() = default;
  FloatArrayLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : FloatArrayLogEntry{log, name, {}, timestamp} {}
  FloatArrayLogEntry(DataLog& log, std::string_view name,
                     std::string_view metadata, int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const float> arr, int64_t timestamp = 0) {
    m_log->AppendFloatArray(m_entry, arr, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::initializer_list<float> arr, int64_t timestamp = 0) {
    Append({arr.begin(), arr.end()}, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::span<const float> arr, int64_t timestamp = 0);

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::initializer_list<float> arr, int64_t timestamp = 0) {
    Update({arr.begin(), arr.end()}, timestamp);
  }
};

/**
 * Log array of double values.
 */
class DoubleArrayLogEntry : public DataLogValueEntryImpl<std::vector<double>> {
 public:
  static constexpr const char* kDataType = "double[]";

  DoubleArrayLogEntry() = default;
  DoubleArrayLogEntry(DataLog& log, std::string_view name,
                      int64_t timestamp = 0)
      : DoubleArrayLogEntry{log, name, {}, timestamp} {}
  DoubleArrayLogEntry(DataLog& log, std::string_view name,
                      std::string_view metadata, int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const double> arr, int64_t timestamp = 0) {
    m_log->AppendDoubleArray(m_entry, arr, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::initializer_list<double> arr, int64_t timestamp = 0) {
    Append({arr.begin(), arr.end()}, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::span<const double> arr, int64_t timestamp = 0);

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::initializer_list<double> arr, int64_t timestamp = 0) {
    Update({arr.begin(), arr.end()}, timestamp);
  }
};

/**
 * Log array of string values.
 */
class StringArrayLogEntry
    : public DataLogValueEntryImpl<std::vector<std::string>> {
 public:
  static constexpr const char* kDataType = "string[]";

  StringArrayLogEntry() = default;
  StringArrayLogEntry(DataLog& log, std::string_view name,
                      int64_t timestamp = 0)
      : StringArrayLogEntry{log, name, {}, timestamp} {}
  StringArrayLogEntry(DataLog& log, std::string_view name,
                      std::string_view metadata, int64_t timestamp = 0)
      : DataLogValueEntryImpl{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const std::string> arr, int64_t timestamp = 0) {
    m_log->AppendStringArray(m_entry, arr, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const std::string_view> arr, int64_t timestamp = 0) {
    m_log->AppendStringArray(m_entry, arr, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::initializer_list<std::string_view> arr,
              int64_t timestamp = 0) {
    Append(std::span<const std::string_view>{arr.begin(), arr.end()},
           timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::span<const std::string> arr, int64_t timestamp = 0);

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::span<const std::string_view> arr, int64_t timestamp = 0);

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::initializer_list<std::string_view> arr,
              int64_t timestamp = 0) {
    Update(std::span<const std::string_view>{arr.begin(), arr.end()},
           timestamp);
  }
};

/**
 * Log raw struct serializable objects.
 */
template <typename T, typename... I>
  requires StructSerializable<T, I...>
class StructLogEntry : public DataLogEntry {
  using S = Struct<T, I...>;

 public:
  StructLogEntry() = default;
  StructLogEntry(DataLog& log, std::string_view name, I... info,
                 int64_t timestamp = 0)
      : StructLogEntry{log, name, {}, std::move(info)..., timestamp} {}
  StructLogEntry(DataLog& log, std::string_view name, std::string_view metadata,
                 I... info, int64_t timestamp = 0)
      : m_info{std::move(info)...} {
    m_log = &log;
    log.AddStructSchema<T, I...>(info..., timestamp);
    m_entry =
        log.Start(name, GetStructTypeString<T>(info...), metadata, timestamp);
  }

  StructLogEntry(StructLogEntry&& rhs)
      : DataLogEntry{std::move(rhs)}, m_info{std::move(rhs.m_info)} {
    std::scoped_lock lock{rhs.m_mutex};
    m_lastValue = std::move(rhs.m_lastValue);
  }
  StructLogEntry& operator=(StructLogEntry&& rhs) {
    DataLogEntry::operator=(std::move(rhs));
    m_info = std::move(rhs.m_info);
    std::scoped_lock lock{m_mutex, rhs.m_mutex};
    m_lastValue = std::move(rhs.m_lastValue);
    return *this;
  }

  /**
   * Appends a record to the log.
   *
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(const T& data, int64_t timestamp = 0) {
    if constexpr (sizeof...(I) == 0) {
      if constexpr (wpi::is_constexpr([] { S::GetSize(); })) {
        uint8_t buf[S::GetSize()];
        S::Pack(buf, data);
        m_log->AppendRaw(m_entry, buf, timestamp);
        return;
      }
    }
    wpi::SmallVector<uint8_t, 128> buf;
    buf.resize_for_overwrite(std::apply(S::GetSize, m_info));
    std::apply([&](const I&... info) { S::Pack(buf, data, info...); }, m_info);
    m_log->AppendRaw(m_entry, buf, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(const T& data, int64_t timestamp = 0) {
    if constexpr (sizeof...(I) == 0) {
      if constexpr (wpi::is_constexpr([] { S::GetSize(); })) {
        uint8_t buf[S::GetSize()];
        S::Pack(buf, data);
        std::scoped_lock lock{m_mutex};
        if (m_lastValue.empty() ||
            !std::equal(buf, buf + S::GetSize(), m_lastValue.begin(),
                        m_lastValue.end())) {
          m_lastValue.assign(buf, buf + S::GetSize());
          m_log->AppendRaw(m_entry, buf, timestamp);
        }
        return;
      }
    }
    wpi::SmallVector<uint8_t, 128> buf;
    buf.resize_for_overwrite(std::apply(S::GetSize, m_info));
    std::apply([&](const I&... info) { S::Pack(buf, data, info...); }, m_info);
    std::scoped_lock lock{m_mutex};
    if (m_lastValue.empty() ||
        !std::equal(buf.begin(), buf.end(), m_lastValue.begin(),
                    m_lastValue.end())) {
      m_lastValue.assign(buf.begin(), buf.end());
      m_log->AppendRaw(m_entry, buf, timestamp);
    }
  }

  /**
   * Gets whether there is a last value.
   *
   * @note The last value is local to this class instance and updated only with
   * Update(), not Append().
   *
   * @return True if last value exists, false otherwise.
   */
  bool HasLastValue() const { return !m_lastValue.empty(); }

  /**
   * Gets the last value.
   *
   * @note The last value is local to this class instance and updated only with
   * Update(), not Append().
   *
   * @return Last value (empty if no last value)
   */
  std::optional<T> GetLastValue() const {
    std::scoped_lock lock{m_mutex};
    if (m_lastValue.empty()) {
      return std::nullopt;
    }
    return std::apply(
        [&](const I&... info) { return S::Unpack(m_lastValue, info...); },
        m_info);
  }

 private:
  mutable wpi::mutex m_mutex;
  std::vector<uint8_t> m_lastValue;
  [[no_unique_address]]
  std::tuple<I...> m_info;
};

/**
 * Log raw struct serializable array of objects.
 */
template <typename T, typename... I>
  requires StructSerializable<T, I...>
class StructArrayLogEntry : public DataLogEntry {
  using S = Struct<T, I...>;

 public:
  StructArrayLogEntry() = default;
  StructArrayLogEntry(DataLog& log, std::string_view name, I... info,
                      int64_t timestamp = 0)
      : StructArrayLogEntry{log, name, {}, std::move(info)..., timestamp} {}
  StructArrayLogEntry(DataLog& log, std::string_view name,
                      std::string_view metadata, I... info,
                      int64_t timestamp = 0)
      : m_info{std::move(info)...} {
    m_log = &log;
    log.AddStructSchema<T, I...>(info..., timestamp);
    m_entry = log.Start(
        name, MakeStructArrayTypeString<T, std::dynamic_extent>(info...),
        metadata, timestamp);
  }

  StructArrayLogEntry(StructArrayLogEntry&& rhs)
      : DataLogEntry{std::move(rhs)},
        m_buf{std::move(rhs.m_buf)},
        m_info{std::move(rhs.m_info)} {
    std::scoped_lock lock{rhs.m_mutex};
    m_lastValue = std::move(rhs.m_lastValue);
  }
  StructArrayLogEntry& operator=(StructArrayLogEntry&& rhs) {
    DataLogEntry::operator=(std::move(rhs));
    m_buf = std::move(rhs.m_buf);
    m_info = std::move(rhs.m_info);
    std::scoped_lock lock{m_mutex, rhs.m_mutex};
    m_lastValue = std::move(rhs.m_lastValue);
    return *this;
  }

  /**
   * Appends a record to the log.
   *
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  template <typename U>
#if __cpp_lib_ranges >= 201911L
    requires std::ranges::range<U> &&
             std::convertible_to<std::ranges::range_value_t<U>, T>
#endif
  void Append(U&& data, int64_t timestamp = 0) {
    std::apply(
        [&](const I&... info) {
          m_buf.Write(
              std::forward<U>(data),
              [&](auto bytes) { m_log->AppendRaw(m_entry, bytes, timestamp); },
              info...);
        },
        m_info);
  }

  /**
   * Appends a record to the log.
   *
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const T> data, int64_t timestamp = 0) {
    std::apply(
        [&](const I&... info) {
          m_buf.Write(
              data,
              [&](auto bytes) { m_log->AppendRaw(m_entry, bytes, timestamp); },
              info...);
        },
        m_info);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(std::span<const T> data, int64_t timestamp = 0) {
    std::apply(
        [&](const I&... info) {
          m_buf.Write(
              data,
              [&](auto bytes) {
                std::scoped_lock lock{m_mutex};
                if (!m_lastValue.has_value()) {
                  m_lastValue = std::vector(bytes.begin(), bytes.end());
                  m_log->AppendRaw(m_entry, bytes, timestamp);
                } else if (!std::equal(bytes.begin(), bytes.end(),
                                       m_lastValue->begin(),
                                       m_lastValue->end())) {
                  m_lastValue->assign(bytes.begin(), bytes.end());
                  m_log->AppendRaw(m_entry, bytes, timestamp);
                }
              },
              info...);
        },
        m_info);
  }

  /**
   * Gets whether there is a last value.
   *
   * @note The last value is local to this class instance and updated only with
   * Update(), not Append().
   *
   * @return True if last value exists, false otherwise.
   */
  bool HasLastValue() const { return m_lastValue.has_value(); }

  /**
   * Gets the last value.
   *
   * @note The last value is local to this class instance and updated only with
   * Update(), not Append().
   *
   * @return Last value (empty if no last value)
   */
  std::optional<std::vector<T>> GetLastValue() const {
    std::scoped_lock lock{m_mutex};
    if (!m_lastValue.has_value()) {
      return std::nullopt;
    }
    auto& lastValue = m_lastValue.value();
    size_t size = std::apply(S::GetSize, m_info);
    std::vector<T> rv;
    rv.reserve(lastValue.size() / size);
    for (auto in = lastValue.begin(), end = lastValue.end(); in < end;
         in += size) {
      std::apply(
          [&](const I&... info) {
            rv.emplace_back(S::Unpack(
                std::span<const uint8_t>{std::to_address(in), size}, info...));
          },
          m_info);
    }
    return rv;
  }

 private:
  mutable wpi::mutex m_mutex;
  StructArrayBuffer<T, I...> m_buf;
  std::optional<std::vector<uint8_t>> m_lastValue;
  [[no_unique_address]]
  std::tuple<I...> m_info;
};

/**
 * Log protobuf serializable objects.
 */
template <ProtobufSerializable T>
class ProtobufLogEntry : public DataLogEntry {
  using P = Protobuf<T>;

 public:
  ProtobufLogEntry() = default;
  ProtobufLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : ProtobufLogEntry{log, name, {}, timestamp} {}
  ProtobufLogEntry(DataLog& log, std::string_view name,
                   std::string_view metadata, int64_t timestamp = 0) {
    m_log = &log;
    log.AddProtobufSchema<T>(m_msg, timestamp);
    m_entry = log.Start(name, m_msg.GetTypeString(), metadata, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(const T& data, int64_t timestamp = 0) {
    SmallVector<uint8_t, 128> buf;
    {
      std::scoped_lock lock{m_mutex};
      m_msg.Pack(buf, data);
    }
    m_log->AppendRaw(m_entry, buf, timestamp);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * @note The last value is local to this class instance; using Update() with
   * two instances pointing to the same underlying log entry name will likely
   * result in unexpected results.
   *
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Update(const T& data, int64_t timestamp = 0) {
    std::scoped_lock lock{m_mutex};
    wpi::SmallVector<uint8_t, 128> buf;
    m_msg.Pack(buf, data);
    if (!m_lastValue.has_value()) {
      m_lastValue = std::vector(buf.begin(), buf.end());
      m_log->AppendRaw(m_entry, buf, timestamp);
    } else if (!std::equal(buf.begin(), buf.end(), m_lastValue->begin(),
                           m_lastValue->end())) {
      m_lastValue->assign(buf.begin(), buf.end());
      m_log->AppendRaw(m_entry, buf, timestamp);
    }
  }

  /**
   * Gets whether there is a last value.
   *
   * @note The last value is local to this class instance and updated only with
   * Update(), not Append().
   *
   * @return True if last value exists, false otherwise.
   */
  bool HasLastValue() const { return m_lastValue.has_value(); }

  /**
   * Gets the last value.
   *
   * @note The last value is local to this class instance and updated only with
   * Update(), not Append().
   *
   * @return Last value (empty if no last value)
   */
  std::optional<T> GetLastValue() const {
    std::scoped_lock lock{m_mutex};
    if (!m_lastValue.has_value()) {
      return std::nullopt;
    }
    return m_msg.Unpack(m_lastValue);
  }

 private:
  mutable wpi::mutex m_mutex;
  ProtobufMessage<T> m_msg;
  std::optional<std::vector<uint8_t>> m_lastValue;
};

}  // namespace wpi::log

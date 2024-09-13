// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
#include <concepts>
#include <functional>
#include <initializer_list>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>
#include <version>

#include "wpi/DenseMap.h"
#include "wpi/SmallVector.h"
#include "wpi/StringMap.h"
#include "wpi/condition_variable.h"
#include "wpi/mutex.h"
#include "wpi/protobuf/Protobuf.h"
#include "wpi/struct/Struct.h"
#include "wpi/timestamp.h"
#endif  // __cplusplus

/**
 * A datalog string (for use with string array).
 */
struct WPI_DataLog_String {
  /** Contents. */
  const char* str;

  /** Length. */
  size_t len;
};

#ifdef __cplusplus

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
 * A data log. The log file is created immediately upon construction with a
 * temporary filename.  The file may be renamed at any time using the
 * SetFilename() function.
 *
 * The lifetime of the data log object must be longer than any data log entry
 * objects that refer to it.
 *
 * The data log is periodically flushed to disk.  It can also be explicitly
 * flushed to disk by using the Flush() function.
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
class DataLog final {
 public:
  /**
   * Construct a new Data Log.  The log will be initially created with a
   * temporary filename.
   *
   * @param dir directory to store the log
   * @param filename filename to use; if none provided, a random filename is
   *                 generated of the form "wpilog_{}.wpilog"
   * @param period time between automatic flushes to disk, in seconds;
   *               this is a time/storage tradeoff
   * @param extraHeader extra header data
   */
  explicit DataLog(std::string_view dir = "", std::string_view filename = "",
                   double period = 0.25, std::string_view extraHeader = "");

  /**
   * Construct a new Data Log.  The log will be initially created with a
   * temporary filename.
   *
   * @param msglog message logger (will be called from separate thread)
   * @param dir directory to store the log
   * @param filename filename to use; if none provided, a random filename is
   *                 generated of the form "wpilog_{}.wpilog"
   * @param period time between automatic flushes to disk, in seconds;
   *               this is a time/storage tradeoff
   * @param extraHeader extra header data
   */
  explicit DataLog(wpi::Logger& msglog, std::string_view dir = "",
                   std::string_view filename = "", double period = 0.25,
                   std::string_view extraHeader = "");

  /**
   * Construct a new Data Log that passes its output to the provided function
   * rather than a file.  The write function will be called on a separate
   * background thread and may block.  The write function is called with an
   * empty data array when the thread is terminating.
   *
   * @param write write function
   * @param period time between automatic calls to write, in seconds;
   *               this is a time/storage tradeoff
   * @param extraHeader extra header data
   */
  explicit DataLog(std::function<void(std::span<const uint8_t> data)> write,
                   double period = 0.25, std::string_view extraHeader = "");

  /**
   * Construct a new Data Log that passes its output to the provided function
   * rather than a file.  The write function will be called on a separate
   * background thread and may block.  The write function is called with an
   * empty data array when the thread is terminating.
   *
   * @param msglog message logger (will be called from separate thread)
   * @param write write function
   * @param period time between automatic calls to write, in seconds;
   *               this is a time/storage tradeoff
   * @param extraHeader extra header data
   */
  explicit DataLog(wpi::Logger& msglog,
                   std::function<void(std::span<const uint8_t> data)> write,
                   double period = 0.25, std::string_view extraHeader = "");

  ~DataLog();
  DataLog(const DataLog&) = delete;
  DataLog& operator=(const DataLog&) = delete;
  DataLog(DataLog&&) = delete;
  DataLog& operator=(const DataLog&&) = delete;

  /**
   * Change log filename.
   *
   * @param filename filename
   */
  void SetFilename(std::string_view filename);

  /**
   * Explicitly flushes the log data to disk.
   */
  void Flush();

  /**
   * Pauses appending of data records to the log.  While paused, no data records
   * are saved (e.g. AppendX is a no-op).  Has no effect on entry starts /
   * finishes / metadata changes.
   */
  void Pause();

  /**
   * Resumes appending of data records to the log.  If called after Stop(),
   * opens a new file (with random name if SetFilename was not called after
   * Stop()) and appends Start records and schema data values for all previously
   * started entries and schemas.
   */
  void Resume();

  /**
   * Stops appending all records to the log, and closes the log file.
   */
  void Stop();

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
  void AppendStringArray(int entry, std::span<const WPI_DataLog_String> arr,
                         int64_t timestamp);

 private:
  struct WriterThreadState;

  void StartLogFile(WriterThreadState& state);
  void WriterThreadMain(std::string_view dir);
  void WriterThreadMain(
      std::function<void(std::span<const uint8_t> data)> write);

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

  wpi::Logger& m_msglog;
  mutable wpi::mutex m_mutex;
  wpi::condition_variable m_cond;
  bool m_doFlush{false};
  bool m_shutdown{false};
  enum State {
    kStart,
    kActive,
    kPaused,
    kStopped,
  } m_state = kActive;
  double m_period;
  std::string m_extraHeader;
  std::string m_newFilename;
  class Buffer;
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
  std::thread m_thread;
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

/**
 * Log arbitrary byte data.
 */
class RawLogEntry : public DataLogEntry {
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
      : DataLogEntry{log, name, type, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::span<const uint8_t> data, int64_t timestamp = 0) {
    m_log->AppendRaw(m_entry, data, timestamp);
  }
};

/**
 * Log boolean values.
 */
class BooleanLogEntry : public DataLogEntry {
 public:
  static constexpr std::string_view kDataType = "boolean";

  BooleanLogEntry() = default;
  BooleanLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : BooleanLogEntry{log, name, {}, timestamp} {}
  BooleanLogEntry(DataLog& log, std::string_view name,
                  std::string_view metadata, int64_t timestamp = 0)
      : DataLogEntry{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(bool value, int64_t timestamp = 0) {
    m_log->AppendBoolean(m_entry, value, timestamp);
  }
};

/**
 * Log integer values.
 */
class IntegerLogEntry : public DataLogEntry {
 public:
  static constexpr std::string_view kDataType = "int64";

  IntegerLogEntry() = default;
  IntegerLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : IntegerLogEntry{log, name, {}, timestamp} {}
  IntegerLogEntry(DataLog& log, std::string_view name,
                  std::string_view metadata, int64_t timestamp = 0)
      : DataLogEntry{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(int64_t value, int64_t timestamp = 0) {
    m_log->AppendInteger(m_entry, value, timestamp);
  }
};

/**
 * Log float values.
 */
class FloatLogEntry : public DataLogEntry {
 public:
  static constexpr std::string_view kDataType = "float";

  FloatLogEntry() = default;
  FloatLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : FloatLogEntry{log, name, {}, timestamp} {}
  FloatLogEntry(DataLog& log, std::string_view name, std::string_view metadata,
                int64_t timestamp = 0)
      : DataLogEntry{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(float value, int64_t timestamp = 0) {
    m_log->AppendFloat(m_entry, value, timestamp);
  }
};

/**
 * Log double values.
 */
class DoubleLogEntry : public DataLogEntry {
 public:
  static constexpr std::string_view kDataType = "double";

  DoubleLogEntry() = default;
  DoubleLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : DoubleLogEntry{log, name, {}, timestamp} {}
  DoubleLogEntry(DataLog& log, std::string_view name, std::string_view metadata,
                 int64_t timestamp = 0)
      : DataLogEntry{log, name, kDataType, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(double value, int64_t timestamp = 0) {
    m_log->AppendDouble(m_entry, value, timestamp);
  }
};

/**
 * Log string values.
 */
class StringLogEntry : public DataLogEntry {
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
      : DataLogEntry{log, name, type, metadata, timestamp} {}

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::string_view value, int64_t timestamp = 0) {
    m_log->AppendString(m_entry, value, timestamp);
  }
};

/**
 * Log array of boolean values.
 */
class BooleanArrayLogEntry : public DataLogEntry {
 public:
  static constexpr const char* kDataType = "boolean[]";

  BooleanArrayLogEntry() = default;
  BooleanArrayLogEntry(DataLog& log, std::string_view name,
                       int64_t timestamp = 0)
      : BooleanArrayLogEntry{log, name, {}, timestamp} {}
  BooleanArrayLogEntry(DataLog& log, std::string_view name,
                       std::string_view metadata, int64_t timestamp = 0)
      : DataLogEntry{log, name, kDataType, metadata, timestamp} {}

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
};

/**
 * Log array of integer values.
 */
class IntegerArrayLogEntry : public DataLogEntry {
 public:
  static constexpr const char* kDataType = "int64[]";

  IntegerArrayLogEntry() = default;
  IntegerArrayLogEntry(DataLog& log, std::string_view name,
                       int64_t timestamp = 0)
      : IntegerArrayLogEntry{log, name, {}, timestamp} {}
  IntegerArrayLogEntry(DataLog& log, std::string_view name,
                       std::string_view metadata, int64_t timestamp = 0)
      : DataLogEntry{log, name, kDataType, metadata, timestamp} {}

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
};

/**
 * Log array of float values.
 */
class FloatArrayLogEntry : public DataLogEntry {
 public:
  static constexpr const char* kDataType = "float[]";

  FloatArrayLogEntry() = default;
  FloatArrayLogEntry(DataLog& log, std::string_view name, int64_t timestamp = 0)
      : FloatArrayLogEntry{log, name, {}, timestamp} {}
  FloatArrayLogEntry(DataLog& log, std::string_view name,
                     std::string_view metadata, int64_t timestamp = 0)
      : DataLogEntry{log, name, kDataType, metadata, timestamp} {}

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
};

/**
 * Log array of double values.
 */
class DoubleArrayLogEntry : public DataLogEntry {
 public:
  static constexpr const char* kDataType = "double[]";

  DoubleArrayLogEntry() = default;
  DoubleArrayLogEntry(DataLog& log, std::string_view name,
                      int64_t timestamp = 0)
      : DoubleArrayLogEntry{log, name, {}, timestamp} {}
  DoubleArrayLogEntry(DataLog& log, std::string_view name,
                      std::string_view metadata, int64_t timestamp = 0)
      : DataLogEntry{log, name, kDataType, metadata, timestamp} {}

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
};

/**
 * Log array of string values.
 */
class StringArrayLogEntry : public DataLogEntry {
 public:
  static constexpr const char* kDataType = "string[]";

  StringArrayLogEntry() = default;
  StringArrayLogEntry(DataLog& log, std::string_view name,
                      int64_t timestamp = 0)
      : StringArrayLogEntry{log, name, {}, timestamp} {}
  StringArrayLogEntry(DataLog& log, std::string_view name,
                      std::string_view metadata, int64_t timestamp = 0)
      : DataLogEntry{log, name, kDataType, metadata, timestamp} {}

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
    m_entry = log.Start(name, S::GetTypeString(info...), metadata, timestamp);
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

 private:
  [[no_unique_address]] std::tuple<I...> m_info;
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

 private:
  StructArrayBuffer<T, I...> m_buf;
  [[no_unique_address]] std::tuple<I...> m_info;
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

 private:
  wpi::mutex m_mutex;
  ProtobufMessage<T> m_msg;
};

}  // namespace wpi::log

extern "C" {
#endif  // __cplusplus

/** C-compatible data log (opaque struct). */
struct WPI_DataLog;

/**
 * Construct a new Data Log.  The log will be initially created with a
 * temporary filename.
 *
 * @param dir directory to store the log
 * @param filename filename to use; if none provided, a random filename is
 *                 generated of the form "wpilog_{}.wpilog"
 * @param period time between automatic flushes to disk, in seconds;
 *               this is a time/storage tradeoff
 * @param extraHeader extra header data
 */
struct WPI_DataLog* WPI_DataLog_Create(const char* dir, const char* filename,
                                       double period, const char* extraHeader);

/**
 * Construct a new Data Log that passes its output to the provided function
 * rather than a file.  The write function will be called on a separate
 * background thread and may block.  The write function is called with an
 * empty data array (data=NULL, len=0) when the thread is terminating.
 *
 * @param write write function
 * @param ptr pointer to pass to write function ptr parameter
 * @param period time between automatic calls to write, in seconds;
 *               this is a time/storage tradeoff
 * @param extraHeader extra header data
 */
struct WPI_DataLog* WPI_DataLog_Create_Func(
    void (*write)(void* ptr, const uint8_t* data, size_t len), void* ptr,
    double period, const char* extraHeader);

/**
 * Releases a data log object. Closes the file and returns resources to the
 * system.
 *
 * @param datalog data log
 */
void WPI_DataLog_Release(struct WPI_DataLog* datalog);

/**
 * Change log filename.
 *
 * @param datalog data log
 * @param filename filename
 */
void WPI_DataLog_SetFilename(struct WPI_DataLog* datalog, const char* filename);

/**
 * Explicitly flushes the log data to disk.
 *
 * @param datalog data log
 */
void WPI_DataLog_Flush(struct WPI_DataLog* datalog);

/**
 * Pauses appending of data records to the log.  While paused, no data records
 * are saved (e.g. AppendX is a no-op).  Has no effect on entry starts /
 * finishes / metadata changes.
 *
 * @param datalog data log
 */
void WPI_DataLog_Pause(struct WPI_DataLog* datalog);

/**
 * Resumes appending of data records to the log.  If called after Stop(),
 * opens a new file (with random name if SetFilename was not called after
 * Stop()) and appends Start records and schema data values for all previously
 * started entries and schemas.
 *
 * @param datalog data log
 */
void WPI_DataLog_Resume(struct WPI_DataLog* datalog);

/**
 * Stops appending all records to the log, and closes the log file.
 *
 * @param datalog data log
 */
void WPI_DataLog_Stop(struct WPI_DataLog* datalog);

/**
 * Start an entry.  Duplicate names are allowed (with the same type), and
 * result in the same index being returned (Start/Finish are reference
 * counted).  A duplicate name with a different type will result in an error
 * message being printed to the console and 0 being returned (which will be
 * ignored by the Append functions).
 *
 * @param datalog data log
 * @param name Name
 * @param type Data type
 * @param metadata Initial metadata (e.g. data properties)
 * @param timestamp Time stamp (may be 0 to indicate now)
 *
 * @return Entry index
 */
int WPI_DataLog_Start(struct WPI_DataLog* datalog, const char* name,
                      const char* type, const char* metadata,
                      int64_t timestamp);

/**
 * Finish an entry.
 *
 * @param datalog data log
 * @param entry Entry index
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_Finish(struct WPI_DataLog* datalog, int entry,
                        int64_t timestamp);

/**
 * Updates the metadata for an entry.
 *
 * @param datalog data log
 * @param entry Entry index
 * @param metadata New metadata for the entry
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_SetMetadata(struct WPI_DataLog* datalog, int entry,
                             const char* metadata, int64_t timestamp);

/**
 * Appends a raw record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param data Byte array to record
 * @param len Length of byte array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendRaw(struct WPI_DataLog* datalog, int entry,
                           const uint8_t* data, size_t len, int64_t timestamp);

/**
 * Appends a boolean record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param value Boolean value to record
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendBoolean(struct WPI_DataLog* datalog, int entry,
                               int value, int64_t timestamp);

/**
 * Appends an integer record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param value Integer value to record
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendInteger(struct WPI_DataLog* datalog, int entry,
                               int64_t value, int64_t timestamp);

/**
 * Appends a float record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param value Float value to record
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendFloat(struct WPI_DataLog* datalog, int entry,
                             float value, int64_t timestamp);

/**
 * Appends a double record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param value Double value to record
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendDouble(struct WPI_DataLog* datalog, int entry,
                              double value, int64_t timestamp);

/**
 * Appends a string record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param value String value to record
 * @param len Length of string
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendString(struct WPI_DataLog* datalog, int entry,
                              const char* value, size_t len, int64_t timestamp);

/**
 * Appends a boolean array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr Boolean array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendBooleanArray(struct WPI_DataLog* datalog, int entry,
                                    const int* arr, size_t len,
                                    int64_t timestamp);

/**
 * Appends a boolean array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr Boolean array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendBooleanArrayByte(struct WPI_DataLog* datalog, int entry,
                                        const uint8_t* arr, size_t len,
                                        int64_t timestamp);

/**
 * Appends an integer array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr Integer array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendIntegerArray(struct WPI_DataLog* datalog, int entry,
                                    const int64_t* arr, size_t len,
                                    int64_t timestamp);

/**
 * Appends a float array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr Float array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendFloatArray(struct WPI_DataLog* datalog, int entry,
                                  const float* arr, size_t len,
                                  int64_t timestamp);

/**
 * Appends a double array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr Double array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendDoubleArray(struct WPI_DataLog* datalog, int entry,
                                   const double* arr, size_t len,
                                   int64_t timestamp);

/**
 * Appends a string array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr String array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendStringArray(struct WPI_DataLog* datalog, int entry,
                                   const WPI_DataLog_String* arr, size_t len,
                                   int64_t timestamp);

void WPI_DataLog_AddSchemaString(struct WPI_DataLog* datalog, const char* name,
                                 const char* type, const char* schema,
                                 int64_t timestamp);

void WPI_DataLog_AddSchema(struct WPI_DataLog* datalog, const char* name,
                           const char* type, const uint8_t* schema,
                           size_t schema_len, int64_t timestamp);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

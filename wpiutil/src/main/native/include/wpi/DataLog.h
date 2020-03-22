// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "wpi/DenseMap.h"
#include "wpi/StringMap.h"
#include "wpi/condition_variable.h"
#include "wpi/mutex.h"
#include "wpi/span.h"

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
  explicit DataLog(std::function<void(wpi::span<const uint8_t> data)> write,
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
                   std::function<void(wpi::span<const uint8_t> data)> write,
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
   * Resumes appending of data records to the log.
   */
  void Resume();

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
   * Appends a record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendRaw(int entry, wpi::span<const uint8_t> data, int64_t timestamp);

  /**
   * Appends a record to the log.
   *
   * @param entry Entry index, as returned by Start()
   * @param data Data to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void AppendRaw2(int entry, wpi::span<const wpi::span<const uint8_t>> data,
                  int64_t timestamp);

  void AppendBoolean(int entry, bool value, int64_t timestamp);
  void AppendInteger(int entry, int64_t value, int64_t timestamp);
  void AppendFloat(int entry, float value, int64_t timestamp);
  void AppendDouble(int entry, double value, int64_t timestamp);
  void AppendString(int entry, std::string_view value, int64_t timestamp);
  void AppendBooleanArray(int entry, wpi::span<const bool> arr,
                          int64_t timestamp);
  void AppendBooleanArray(int entry, wpi::span<const int> arr,
                          int64_t timestamp);
  void AppendBooleanArray(int entry, wpi::span<const uint8_t> arr,
                          int64_t timestamp);
  void AppendIntegerArray(int entry, wpi::span<const int64_t> arr,
                          int64_t timestamp);
  void AppendFloatArray(int entry, wpi::span<const float> arr,
                        int64_t timestamp);
  void AppendDoubleArray(int entry, wpi::span<const double> arr,
                         int64_t timestamp);
  void AppendStringArray(int entry, wpi::span<const std::string> arr,
                         int64_t timestamp);
  void AppendStringArray(int entry, wpi::span<const std::string_view> arr,
                         int64_t timestamp);

 private:
  void WriterThreadMain(std::string_view dir);
  void WriterThreadMain(
      std::function<void(wpi::span<const uint8_t> data)> write);

  // must be called with m_mutex held
  uint8_t* StartRecord(uint32_t entry, uint64_t timestamp, uint32_t payloadSize,
                       size_t reserveSize);
  uint8_t* Reserve(size_t size);
  void AppendImpl(wpi::span<const uint8_t> data);
  void AppendStringImpl(std::string_view str);

  wpi::Logger& m_msglog;
  mutable wpi::mutex m_mutex;
  wpi::condition_variable m_cond;
  bool m_active{true};
  bool m_doFlush{false};
  bool m_paused{false};
  double m_period;
  std::string m_extraHeader;
  std::string m_newFilename;
  class Buffer;
  std::vector<Buffer> m_free;
  std::vector<Buffer> m_outgoing;
  struct EntryInfo {
    std::string type;
    int id{0};
  };
  wpi::StringMap<EntryInfo> m_entries;
  wpi::DenseMap<int, unsigned int> m_entryCounts;
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
  void Append(wpi::span<const uint8_t> data, int64_t timestamp = 0) {
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
  void Append(wpi::span<const bool> arr, int64_t timestamp = 0) {
    m_log->AppendBooleanArray(m_entry, arr, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::initializer_list<bool> arr, int64_t timestamp = 0) {
    Append(wpi::span{arr.begin(), arr.end()}, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(wpi::span<const int> arr, int64_t timestamp = 0) {
    m_log->AppendBooleanArray(m_entry, arr, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(std::initializer_list<int> arr, int64_t timestamp = 0) {
    Append(wpi::span{arr.begin(), arr.end()}, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(wpi::span<const uint8_t> arr, int64_t timestamp = 0) {
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
  void Append(wpi::span<const int64_t> arr, int64_t timestamp = 0) {
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
  void Append(wpi::span<const float> arr, int64_t timestamp = 0) {
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
  void Append(wpi::span<const double> arr, int64_t timestamp = 0) {
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
  void Append(wpi::span<const std::string> arr, int64_t timestamp = 0) {
    m_log->AppendStringArray(m_entry, arr, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param arr Values to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  void Append(wpi::span<const std::string_view> arr, int64_t timestamp = 0) {
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
    Append(wpi::span<const std::string_view>{arr.begin(), arr.end()},
           timestamp);
  }
};

}  // namespace wpi::log

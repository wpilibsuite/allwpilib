/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <iterator>
#include <memory>
#include <string>
#include <system_error>
#include <utility>

#include "wpi/ArrayRef.h"
#include "wpi/Error.h"
#include "wpi/StringRef.h"
#include "wpi/Twine.h"

namespace wpi {
template <typename T>
class SmallVectorImpl;
}  // namespace wpi

namespace wpi {
namespace log {

template <typename T>
struct DataLogInfo {
  // static T Read(wpi::ArrayRef<uint8_t> data);
  // static bool Append(DataLogImpl& impl, uint64_t timestamp, T val);
};

// Common iterator bits
template <typename Container, typename Value>
class ConstArrayIterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = Value;
  using difference_type = std::ptrdiff_t;
  using pointer = const value_type*;
  using reference = const value_type&;

 private:
  /// A proxy object for computing a reference via indirecting a copy of an
  /// iterator. This is used in APIs which need to produce a reference via
  /// indirection but for which the iterator object might be a temporary. The
  /// proxy preserves the iterator internally and exposes the indirected
  /// reference via a conversion operator.
  class ReferenceProxy {
    friend ConstArrayIterator;

    ConstArrayIterator it;

    explicit ReferenceProxy(ConstArrayIterator it) : it(std::move(it)) {}

   public:
    operator reference() const { return *it; }
  };

 public:
  ConstArrayIterator(const Container* container, size_t n)
      : m_container(container), m_index(n) {}

  bool operator==(const ConstArrayIterator& oth) const {
    return m_container == oth.m_container && m_index == oth.m_index;
  }
  bool operator!=(const ConstArrayIterator& oth) const {
    return !this->operator==(oth);
  }

  bool operator<(const ConstArrayIterator& oth) const {
    return m_index < oth.m_index;
  }
  bool operator>(const ConstArrayIterator& oth) const {
    return !this->operator<(oth) && !this->operator==(oth);
  }
  bool operator<=(const ConstArrayIterator& oth) const {
    return !this->operator>(oth);
  }
  bool operator>=(const ConstArrayIterator& oth) const {
    return !this->operator<(oth);
  }

  std::ptrdiff_t operator-(const ConstArrayIterator& oth) const {
    return oth.m_index - m_index;
  }

  ConstArrayIterator& operator+=(difference_type n) {
    m_index += n;
    m_validValue = false;
    return *this;
  }

  ConstArrayIterator& operator-=(difference_type n) {
    m_index -= n;
    m_validValue = false;
    return *this;
  }

  ConstArrayIterator operator+(difference_type n) const {
    ConstArrayIterator tmp = *this;
    tmp += n;
    return tmp;
  }

  friend ConstArrayIterator operator+(difference_type n,
                                      const ConstArrayIterator& i) {
    return i + n;
  }

  ConstArrayIterator operator-(difference_type n) const {
    ConstArrayIterator tmp = *this;
    tmp -= n;
    return tmp;
  }

  ConstArrayIterator& operator++() { return this->operator+=(1); }

  ConstArrayIterator operator++(int) {
    ConstArrayIterator tmp = *this;
    ++*this;
    return tmp;
  }

  ConstArrayIterator& operator--() { return this->operator-=(1); }

  ConstArrayIterator operator--(int) {
    ConstArrayIterator tmp = *this;
    --*this;
    return tmp;
  }

  reference operator*() const {
    if (!m_validValue) {
      m_value = m_container->operator[](m_index);
      m_validValue = true;
    }
    return m_value;
  }

  pointer operator->() const { return &this->operator*(); }

  ReferenceProxy operator[](difference_type n) const {
    return ReferenceProxy(this->operator+(n));
  }

 private:
  const Container* m_container;
  size_t m_index;
  mutable bool m_validValue = false;
  mutable Value m_value;
};

class MappedFile {
 public:
  MappedFile() = default;
  MappedFile(int fd, size_t length, uint64_t offset, bool readOnly,
             std::error_code& ec);
  ~MappedFile() { Unmap(); }

  MappedFile(const MappedFile&) = delete;
  MappedFile& operator=(const MappedFile&) = delete;

  MappedFile(MappedFile&& rhs) : m_size(rhs.m_size), m_mapping(rhs.m_mapping) {
    rhs.m_mapping = nullptr;
#ifdef _WIN32
    m_fileHandle = rhs.m_fileHandle;
    rhs.m_fileHandle = nullptr;
#endif
  }

  MappedFile& operator=(MappedFile&& rhs) {
    if (m_mapping) {
      Unmap();
    }
    m_size = rhs.m_size;
    m_mapping = rhs.m_mapping;
    rhs.m_mapping = nullptr;
#ifdef _WIN32
    m_fileHandle = rhs.m_fileHandle;
    rhs.m_fileHandle = nullptr;
#endif
    return *this;
  }

  explicit operator bool() const { return m_mapping != nullptr; }

  void Flush();
  void Unmap();

  size_t size() const { return m_size; }
  uint8_t* data() const { return static_cast<uint8_t*>(m_mapping); }
  const uint8_t* const_data() const {
    return static_cast<const uint8_t*>(m_mapping);
  }

 private:
  size_t m_size = 0;
  void* m_mapping = nullptr;
#ifdef _WIN32
  void* m_fileHandle = nullptr;
#endif
};

/**
 * Creation disposition.  Identical to wpi::sys::fs::CreationDisposition
 * but declared here to avoid header dependency.
 */
enum CreationDisposition : unsigned {
  /// CD_CreateAlways - When opening a file:
  ///   * If it already exists, truncate it.
  ///   * If it does not already exist, create a new file.
  CD_CreateAlways = 0,

  /// CD_CreateNew - When opening a file:
  ///   * If it already exists, fail.
  ///   * If it does not already exist, create a new file.
  CD_CreateNew = 1,

  /// CD_OpenExisting - When opening a file:
  ///   * If it already exists, open the file.
  ///   * If it does not already exist, fail.
  CD_OpenExisting = 2,

  /// CD_OpenAlways - When opening a file:
  ///   * If it already exists, open the file.
  ///   * If it does not already exist, create a new file.
  CD_OpenAlways = 3,
};

// Common log bits
class DataLogImpl {
 public:
  DataLogImpl();
  ~DataLogImpl();
  DataLogImpl(const DataLogImpl&) = delete;
  DataLogImpl& operator=(const DataLogImpl&) = delete;

  /**
   * Log implementation configuration.  These can have substantial performance
   * impacts.  The defaults should be good for desktop applications.
   */
  struct Config {
    /**
     * Start out timestamp file with space for this many records.  Note the
     * actual file size will start out this big, but it's a sparse file.
     */
    size_t initialSize = 1000;

    /**
     * Once size has reached this size, grow by this number of records each
     * time.  Prior to it reaching this size, the space is doubled.
     */
    size_t maxGrowSize = 60000;

    /**
     * Maximum map window size.  Larger is more efficient, but may have
     * issues on 32-bit systems.  Defaults to unlimited.
     */
    size_t maxMapSize = 0;

    /**
     * Periodic flush setting.  Flushes log to disk every N appends.
     * Defaults to no periodic flush.
     */
    size_t periodicFlush = 0;

    /**
     * Start out data file with space for this many bytes.  Note the
     * actual file size will start out this big, but it's a sparse file.
     */
    uint64_t initialDataSize = 100000;

    /**
     * Once data file has reached this size, grow by this number of bytes each
     * time.  Prior to it reaching this size, the space is doubled.
     */
    uint64_t maxDataGrowSize = 1024 * 1024;

    /**
     * Use large (e.g. 64-bit) variable-sized data files when creating a new
     * log.  The default is to use 32-bit sizes for the variable-sized data.
     */
    bool largeData = false;

    /**
     * Fill data to put in between each record of variable-sized data in data
     * file.  Useful for e.g. making strings null terminated.  Defaults to
     * nothing.
     */
    wpi::StringRef gapData;

    /**
     * Check data type when opening existing file.  Defaults to checking.
     */
    bool checkType = true;

    /**
     * Check record size when opening existing file.  Defaults to checking.
     */
    bool checkSize = true;

    /**
     * Check data layout when opening existing file.  Defaults to checking.
     */
    bool checkLayout = true;

    /**
     * Check timestamp is monotonically increasing and don't save the value if
     * timestamp decreased.  Defaults to true.
     */
    bool checkMonotonic = true;

    /**
     * Open file in read-only mode.
     */
    bool readOnly = false;
  };

  /**
   * Returns the number of records stored in the log.
   *
   * @return Size of the log, in records
   */
  size_t size() const { return (m_time.writePos - kHeaderSize) / m_recordSize; }

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @return Pair of timestamp and reference to raw stored data
   */
  std::pair<uint64_t, wpi::ArrayRef<uint8_t>> ReadRaw(size_t n) const;

  /**
   * Flushes the log data to disk.
   */
  void Flush();

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.  If it is not, and the configuration
   * has checkMonotonic true, this function will return false.
   *
   * @param timestamp Time stamp
   * @param data Data to record
   * @return True if successful, false on failure
   */
  bool AppendRaw(uint64_t timestamp, wpi::ArrayRef<uint8_t> data);

  /**
   * Starts appending a record to the log.  This allows more complex write
   * logic than AppendRaw() without having to copy the data into a temporary
   * buffer.  As with AppendRaw(), the timestamp must be monotonically
   * increasing if checkMonotonic is true (if it is not, nullptr is returned).
   *
   * AppendRawFinish MUST be called (if this function does not return nullptr)
   * for the data to actually be preserved.
   *
   * @param timestamp Time stamp
   * @param size length of data to record (ignored if records are fixed size)
   * @return Pointer to start of writable data region, or nullptr if error
   */
  uint8_t* AppendRawStart(uint64_t timestamp, uint64_t size);

  /**
   * Finishes appending a record to the log started with AppendRawStart().
   * Size must be the same as the size passed to AppendRawStart().
   *
   * @param size length of data recorded (ignored if records are fixed size)
   */
  void AppendRawFinish(uint64_t size);

  /**
   * Finds the first record with timestamp greater than or equal to the
   * provided time.
   *
   * @param timestamp Time stamp
   * @param first index of first record
   * @param last index of last record
   * @return Index of record
   */
  size_t FindImpl(uint64_t timestamp, size_t first = 0,
                  size_t last = SIZE_MAX) const;

  /**
   * Checks the currently opened log file header.
   *
   * @param dataType Data type
   * @param dataLayout Data layout
   * @param recordSize Record size; 0 indicates variable data size
   * @param checkType Check data type matches
   * @param checkLayout Check data layout matches
   * @param checkSize Check size matches
   * @return Error if mismatch, wpi::Error::success() if no error
   */
  wpi::Error Check(const wpi::Twine& dataType, const wpi::Twine& dataLayout,
                   unsigned int recordSize, bool checkType, bool checkLayout,
                   bool checkSize) const;

  /**
   * Opens or creates a log file.  If file exists, returns error if file header
   * does not match the dataType, dataLayout, and/or recordSize specified
   * (if these checks are enabled in the config parameter).
   *
   * @param filename Filename to open
   * @param dataType Data type
   * @param dataLayout Data layout
   * @param recordSize Record size; 0 indicates variable data size.
   * @param disp Creation disposition (e.g. open or create)
   * @param config Implementation configuration
   * @return Error (wpi::Error::success() if no error)
   */
  wpi::Error DoOpen(const wpi::Twine& filename, const wpi::Twine& dataType,
                    const wpi::Twine& dataLayout, unsigned int recordSize,
                    CreationDisposition disp, const Config& config);

  std::string m_dataType;
  std::string m_dataLayout;
  unsigned int m_recordSize = 0;
  bool m_fixedSize = false;
  size_t m_periodicFlush = 0;
  size_t m_periodicFlushCount = 0;
  std::string m_gapData;
  bool m_checkMonotonic = true;
  uint64_t m_lastTimestamp = 0;

  static constexpr size_t kTimestampSize = 8;

 private:
  wpi::Error ReadHeader();
  void WriteHeader();

  static constexpr size_t kHeaderSize = 4096;

  struct FileInfo {
    FileInfo() = default;
    FileInfo(const FileInfo&) = delete;
    FileInfo& operator=(const FileInfo&) = delete;
    ~FileInfo() { Close(); }

    std::error_code Open(const wpi::Twine& filename, CreationDisposition disp,
                         bool ro);
    void Close();

    // this remaps the map if necessary
    size_t GetMappedOffset(uint64_t pos, size_t len, std::error_code& ec) const;

    wpi::ArrayRef<uint8_t> Read(uint64_t pos, size_t len) const;
    void Write(uint64_t pos, wpi::ArrayRef<uint8_t> data);

    int fd = -1;

    mutable MappedFile map;
    mutable uint64_t mapOffset = 0;
    uint64_t maxGrowSize = 0;
    mutable uint64_t mapGrowSize = 0;
    size_t maxMapSize = 0;
    mutable uint64_t fileSize = 0;
    uint64_t writePos = 0;
    bool readOnly = false;
  };

  FileInfo m_time;
  FileInfo m_data;
};

/**
 * CRTP base class for data log classes.
 *
 * Derived classes need to implement operator[] and should implement an
 * appropriate Append() function.
 */
template <typename Derived, typename Value>
class DataLogBase {
 public:
  DataLogBase() = default;
  DataLogBase(const DataLogBase&) = delete;
  DataLogBase& operator=(const DataLogBase&) = delete;
  DataLogBase(DataLogBase&& rhs) : m_impl(rhs.m_impl), m_owned(rhs.m_owned) {
    rhs.m_impl = nullptr;
  }
  DataLogBase& operator=(DataLogBase&& rhs) {
    m_impl = rhs.m_impl;
    m_owned = rhs.m_owned;
    rhs.m_impl = nullptr;
  }
  ~DataLogBase() {
    if (m_owned) {
      delete m_impl;
    }
  }

  explicit operator bool() const { return m_impl != nullptr; }

  using Config = DataLogImpl::Config;

  using iterator = ConstArrayIterator<Derived, std::pair<uint64_t, Value>>;

  /**
   * Gets the data type string.  This is persistent and saved as part of the
   * file header.
   *
   * @return Data type
   */
  wpi::StringRef GetDataType() const { return m_impl->m_dataType; }

  /**
   * Gets the data layout string.  This is persistent and saved as part of the
   * file header.
   *
   * @return Data layout
   */
  wpi::StringRef GetDataLayout() const { return m_impl->m_dataLayout; }

  /**
   * Gets the record size.  Note this returns a non-zero value even if the
   * stored data is of variable length.  The record size includes the
   * timestamp.
   *
   * @return Record size, in bytes
   */
  unsigned int GetRecordSize() const { return m_impl->m_recordSize; }

  /**
   * Returns whether the stored data is fixed length.
   *
   * @return True if fixed length, false if variable length.
   */
  bool IsFixedSize() const { return m_impl->m_fixedSize; }

  /**
   * Flushes the log data to disk.
   */
  void Flush() { m_impl->Flush(); }

  DataLogImpl* GetImpl() { return m_impl; }
  const DataLogImpl* GetImpl() const { return m_impl; }

  bool empty() const { return m_impl->size() == 0; }

  /**
   * Returns the number of records stored in the log.
   *
   * @return Size of the log, in records
   */
  size_t size() const { return m_impl->size(); }

  std::pair<uint64_t, Value> front() const {
    return static_cast<const Derived*>(this)->operator[](0);
  }

  std::pair<uint64_t, Value> back() const {
    return static_cast<const Derived*>(this)->operator[](m_impl->size() - 1);
  }

  iterator begin() const {
    return iterator(static_cast<const Derived*>(this), 0);
  }

  iterator end() const {
    return iterator(static_cast<const Derived*>(this), m_impl->size());
  }

  iterator find(uint64_t timestamp) const {
    return iterator(static_cast<const Derived*>(this),
                    m_impl->FindImpl(timestamp));
  }

  iterator find(uint64_t timestamp, iterator first, iterator last) const {
    iterator b = begin();
    return iterator(static_cast<const Derived*>(this),
                    m_impl->FindImpl(timestamp, first - b, last - b));
  }

 protected:
  DataLogBase(DataLogImpl* impl, bool owned) : m_impl(impl), m_owned(owned) {}

  DataLogImpl* m_impl = nullptr;
  bool m_owned = true;

  static constexpr unsigned int kTimestampSize = DataLogImpl::kTimestampSize;
};

/**
 * Log arbitrary byte data.
 */
class DataLog : public DataLogBase<DataLog, wpi::ArrayRef<uint8_t>> {
 public:
  DataLog() = default;

  /**
   * Opens a log file.  Fails if file does not exist.  Does not check dataType,
   * dataLayout, or recordSize of opened file.
   *
   * @param filename Filename to open
   * @param config Implementation configuration
   * @return Log object (or error)
   */
  static wpi::Expected<DataLog> Open(const wpi::Twine& filename,
                                     const Config& config = {});

  /**
   * Opens or creates a log file.  If file exists, returns error if file header
   * does not match the dataType, dataLayout, and/or recordSize specified
   * (if these checks are enabled in the config parameter).
   *
   * @param filename Filename to open
   * @param dataType Data type
   * @param dataLayout Data layout
   * @param recordSize Record size; 0 indicates variable data size
   * @param disp Creation disposition (e.g. open or create)
   * @param config Implementation configuration
   * @return Log object (or error)
   */
  static wpi::Expected<DataLog> Open(const wpi::Twine& filename,
                                     const wpi::Twine& dataType,
                                     const wpi::Twine& dataLayout,
                                     unsigned int recordSize,
                                     CreationDisposition disp,
                                     const Config& config = {});

  /**
   * Makes a reference to the log.  Named WrapUnsafe for compatibility with
   * specific log types.
   *
   * @note Keeps a reference to passed-in object.  Lifetime of datalog must
   *       extend past the lifetime of return value.
   *
   * @param datalog data log object
   * @return Log object
   */
  static DataLog WrapUnsafe(DataLog& datalog) {
    return DataLog(datalog.GetImpl(), false);
  }

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param data Data to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, wpi::ArrayRef<uint8_t> data) {
    return m_impl->AppendRaw(timestamp, data);
  }

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @return Pair of timestamp and reference to raw stored data
   */
  std::pair<uint64_t, wpi::ArrayRef<uint8_t>> operator[](size_t n) const {
    return m_impl->ReadRaw(n);
  }

 protected:
  DataLog(DataLogImpl* ptr, bool owned) : DataLogBase(ptr, owned) {}
};

template <typename Derived>
class DataLogStaticMixin {
 public:
  /**
   * Wraps a generic data log.  Returns log object, or error if log is
   * not a log of this type.
   *
   * @note Keeps a reference to passed-in object.  Lifetime of datalog must
   *       extend past the lifetime of return value.
   *
   * @param datalog data log object
   * @param layout check layout matches
   * @return Log object (or error)
   */
  static wpi::Expected<Derived> Wrap(DataLog& datalog,
                                     bool checkLayout = false) {
    auto impl = datalog.GetImpl();
    if (wpi::Error e =
            impl->Check(Derived::kDataType, Derived::kDataLayout,
                        Derived::kRecordSize, true, checkLayout, true)) {
      return wpi::Expected<Derived> { std::move(e) }
    };
    return Derived(impl, false);
  }

  /**
   * Wraps a generic data log without doing format checks.
   *
   * @note Keeps a reference to passed-in object.  Lifetime of datalog must
   *       extend past the lifetime of return value.
   *
   * @param datalog data log object
   * @return Log object
   */
  static Derived WrapUnsafe(DataLog& datalog) {
    return Derived(datalog.GetImpl(), false);
  }

  /**
   * Opens a log file.  Returns log object, or error if file does not exist.
   *
   * @param filename Filename to open
   * @param disp Creation disposition (e.g. open or create)
   * @param config Implementation configuration
   * @return Log object (or error)
   */
  static wpi::Expected<Derived> Open(const wpi::Twine& filename,
                                     CreationDisposition disp,
                                     const DataLogImpl::Config& config = {}) {
    Derived log(new DataLogImpl, true);
    if (wpi::Error e = log.GetImpl()->DoOpen(
            filename, Derived::kDataType, Derived::kDataLayout,
            Derived::kRecordSize, disp, config)) {
      return wpi::Expected<Derived> { std::move(e) }
    };
    return wpi::Expected<Derived>{std::move(log)};

  }
};

/**
 * Log boolean values.
 */
class BooleanLog : public DataLogBase<BooleanLog, bool>,
                   public DataLogStaticMixin<BooleanLog> {
  friend class DataLogStaticMixin<BooleanLog>;

 public:
  static constexpr const char* kDataType = "boolean";
  static constexpr const char* kDataLayout = "byte";
  static constexpr unsigned int kRecordSize = kTimestampSize + 1;

  BooleanLog() = default;

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param value Value to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, bool value) {
    uint8_t buf[1];
    buf[0] = value ? 1 : 0;
    return m_impl->AppendRaw(timestamp, buf);
  }

  /**
   * Reads the raw stored data.
   *
   * @param n Log index
   * @return Pair of timestamp and double value
   */
  std::pair<uint64_t, bool> operator[](size_t n) const {
    auto [ts, arr] = m_impl->ReadRaw(n);
    return {ts, arr[0] != 0};
  }

 protected:
  BooleanLog(DataLogImpl* ptr, bool owned) : DataLogBase(ptr, owned) {}
};

/**
 * Log double values.
 */
class DoubleLog : public DataLogBase<DoubleLog, double>,
                  public DataLogStaticMixin<DoubleLog> {
  friend class DataLogStaticMixin<DoubleLog>;

 public:
  static constexpr const char* kDataType = "double";
  static constexpr const char* kDataLayout = "float64";
  static constexpr unsigned int kRecordSize = kTimestampSize + 8;

  DoubleLog() = default;

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param value Value to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, double value);

  /**
   * Reads the raw stored data.
   *
   * @param n Log index
   * @return Pair of timestamp and double value
   */
  std::pair<uint64_t, double> operator[](size_t n) const;

 protected:
  DoubleLog(DataLogImpl* ptr, bool owned) : DataLogBase(ptr, owned) {}
};

/**
 * Log string values.
 */
class StringLog : public DataLogBase<StringLog, wpi::StringRef>,
                  public DataLogStaticMixin<StringLog> {
  friend class DataLogStaticMixin<StringLog>;

 public:
  static constexpr const char* kDataType = "string";
  static constexpr const char* kDataLayout = "utf8";
  static constexpr unsigned int kRecordSize = 0;

  StringLog() = default;

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param value Value to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, wpi::StringRef value) {
    return m_impl->AppendRaw(
        timestamp,
        wpi::makeArrayRef(reinterpret_cast<const uint8_t*>(value.data()),
                          value.size()));
  }

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @return Pair of timestamp and string value
   */
  std::pair<uint64_t, wpi::StringRef> operator[](size_t n) const {
    auto [ts, arr] = m_impl->ReadRaw(n);
    return {ts, wpi::StringRef(reinterpret_cast<const char*>(arr.data()),
                               arr.size())};
  }

 protected:
  StringLog(DataLogImpl* ptr, bool owned) : DataLogBase(ptr, owned) {}
};

/**
 * Proxy object for boolean array log access.
 */
class BooleanArrayLogArrayProxy {
  friend class BooleanArrayLog;

 public:
  BooleanArrayLogArrayProxy() = default;

  using iterator = ConstArrayIterator<BooleanArrayLogArrayProxy, bool>;

  iterator begin() const { return iterator(this, 0); }
  iterator end() const { return iterator(this, size()); }

  size_t size() const { return m_data.size(); }

  bool operator[](size_t n) const { return m_data[n] != 0; }

 private:
  explicit BooleanArrayLogArrayProxy(wpi::ArrayRef<uint8_t> data)
      : m_data(data) {}

  wpi::ArrayRef<uint8_t> m_data;
};

/**
 * Log array of boolean values.
 */
class BooleanArrayLog
    : public DataLogBase<BooleanArrayLog, BooleanArrayLogArrayProxy>,
      public DataLogStaticMixin<BooleanArrayLog> {
  friend class DataLogStaticMixin<BooleanArrayLog>;

 public:
  static constexpr const char* kDataType = "boolean[]";
  static constexpr const char* kDataLayout = "byte[]";
  static constexpr unsigned int kRecordSize = 0;

  BooleanArrayLog() = default;

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param arr Values to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, wpi::ArrayRef<bool> arr);

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param arr Values to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, std::initializer_list<bool> arr) {
    return Append(timestamp, wpi::makeArrayRef(arr.begin(), arr.end()));
  }

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param arr Values to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, wpi::ArrayRef<int> arr);

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param arr Values to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, std::initializer_list<int> arr) {
    return Append(timestamp, wpi::makeArrayRef(arr.begin(), arr.end()));
  }

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @param buf Buffer for stored data
   * @return Pair of timestamp and boolean array value
   */
  std::pair<uint64_t, wpi::ArrayRef<bool>> Get(
      size_t n, wpi::SmallVectorImpl<bool>& buf) const;

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @param buf Buffer for stored data
   * @return Pair of timestamp and boolean array value
   */
  std::pair<uint64_t, wpi::ArrayRef<int>> Get(
      size_t n, wpi::SmallVectorImpl<int>& buf) const;

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @return Pair of timestamp and boolean array value
   */
  std::pair<uint64_t, BooleanArrayLogArrayProxy> operator[](size_t n) const {
    auto [ts, arr] = m_impl->ReadRaw(n);
    return {ts, BooleanArrayLogArrayProxy(arr)};
  }

 protected:
  BooleanArrayLog(DataLogImpl* ptr, bool owned) : DataLogBase(ptr, owned) {}
};

/**
 * Proxy object for double array log access.
 */
class DoubleArrayLogArrayProxy {
  friend class DoubleArrayLog;

 public:
  DoubleArrayLogArrayProxy() = default;

  using iterator = ConstArrayIterator<DoubleArrayLogArrayProxy, double>;

  iterator begin() const { return iterator(this, 0); }
  iterator end() const { return iterator(this, size()); }

  size_t size() const { return m_data.size() / 8; }

  double operator[](size_t n) const;

 private:
  explicit DoubleArrayLogArrayProxy(wpi::ArrayRef<uint8_t> data)
      : m_data(data) {}

  wpi::ArrayRef<uint8_t> m_data;
};

/**
 * Log array of double values.
 */
class DoubleArrayLog
    : public DataLogBase<DoubleArrayLog, DoubleArrayLogArrayProxy>,
      public DataLogStaticMixin<DoubleArrayLog> {
  friend class DataLogStaticMixin<DoubleArrayLog>;

 public:
  static constexpr const char* kDataType = "double[]";
  static constexpr const char* kDataLayout = "float64[]";
  static constexpr unsigned int kRecordSize = 0;

  DoubleArrayLog() = default;

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param arr Values to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, wpi::ArrayRef<double> arr);

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param arr Values to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, std::initializer_list<double> arr) {
    return Append(timestamp, wpi::makeArrayRef(arr.begin(), arr.end()));
  }

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @param buf Buffer for stored data
   * @return Pair of timestamp and double array value
   */
  std::pair<uint64_t, wpi::ArrayRef<double>> Get(
      size_t n, wpi::SmallVectorImpl<double>& buf) const;

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @return Pair of timestamp and double array value
   */
  std::pair<uint64_t, DoubleArrayLogArrayProxy> operator[](size_t n) const {
    auto [ts, arr] = m_impl->ReadRaw(n);
    return {ts, DoubleArrayLogArrayProxy(arr)};
  }

 protected:
  DoubleArrayLog(DataLogImpl* ptr, bool owned) : DataLogBase(ptr, owned) {}
};

/**
 * Proxy object for string array log access.
 */
class StringArrayLogArrayProxy {
  friend class StringArrayLog;

 public:
  StringArrayLogArrayProxy() = default;

  using iterator = ConstArrayIterator<StringArrayLogArrayProxy, wpi::StringRef>;

  iterator begin() const { return iterator(this, 0); }
  iterator end() const { return iterator(this, size()); }

  size_t size() const { return m_size; }

  wpi::StringRef operator[](size_t n) const;

 private:
  explicit StringArrayLogArrayProxy(uint32_t size, wpi::ArrayRef<uint8_t> data)
      : m_size(size), m_data(data) {}

  uint32_t m_size;
  wpi::ArrayRef<uint8_t> m_data;
};

/**
 * Log array of double values.
 */
class StringArrayLog
    : public DataLogBase<StringArrayLog, StringArrayLogArrayProxy>,
      public DataLogStaticMixin<StringArrayLog> {
  friend class DataLogStaticMixin<StringArrayLog>;

 public:
  static constexpr const char* kDataType = "string[]";
  static constexpr const char* kDataLayout =
      "u32 n,{u32 off,u32 size}[],utf8[]";
  static constexpr unsigned int kRecordSize = 0;

  StringArrayLog() = default;

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param arr Values to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, wpi::ArrayRef<std::string> arr);

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param arr Values to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, wpi::ArrayRef<wpi::StringRef> arr);

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.
   *
   * @param timestamp Time stamp
   * @param arr Values to record
   * @return True if successful, false on failure
   */
  bool Append(uint64_t timestamp, std::initializer_list<wpi::StringRef> arr) {
    return Append(timestamp, wpi::makeArrayRef(arr.begin(), arr.end()));
  }

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @param buf Buffer for stored data
   * @return Pair of timestamp and string array value
   */
  std::pair<uint64_t, wpi::ArrayRef<std::string>> Get(
      size_t n, wpi::SmallVectorImpl<std::string>& buf) const;

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @param buf Buffer for stored data
   * @return Pair of timestamp and string array value
   */
  std::pair<uint64_t, wpi::ArrayRef<wpi::StringRef>> Get(
      size_t n, wpi::SmallVectorImpl<wpi::StringRef>& buf) const;

  /**
   * Reads the raw stored data.
   *
   * @note The returned reference may be invalidated when log records are
   *       appended.
   *
   * @param n Log index
   * @return Pair of timestamp and string array value
   */
  std::pair<uint64_t, StringArrayLogArrayProxy> operator[](size_t n) const;

 protected:
  StringArrayLog(DataLogImpl* ptr, bool owned) : DataLogBase(ptr, owned) {}
};

}  // namespace log
}  // namespace wpi

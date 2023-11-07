// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <iterator>
#include <memory>
#include <span>
#include <utility>
#include <vector>

#include "wpi/MemoryBuffer.h"

namespace wpi::log {

/**
 * Data contained in a start control record as created by DataLog::Start() when
 * writing the log. This can be read by calling DataLogRecord::GetStartData().
 */
struct StartRecordData {
  /** Entry ID; this will be used for this entry in future records. */
  int entry;

  /** Entry name. */
  std::string_view name;

  /** Type of the stored data for this entry, as a string, e.g. "double". */
  std::string_view type;

  /** Initial metadata. */
  std::string_view metadata;
};

/**
 * Data contained in a set metadata control record as created by
 * DataLog::SetMetadata(). This can be read by calling
 * DataLogRecord::GetSetMetadataData().
 */
struct MetadataRecordData {
  /** Entry ID. */
  int entry;

  /** New metadata for the entry. */
  std::string_view metadata;
};

/**
 * A record in the data log. May represent either a control record (entry == 0)
 * or a data record. Used only for reading (e.g. with DataLogReader).
 */
class DataLogRecord {
 public:
  DataLogRecord() = default;
  DataLogRecord(int entry, int64_t timestamp, std::span<const uint8_t> data)
      : m_timestamp{timestamp}, m_data{data}, m_entry{entry} {}

  /**
   * Gets the entry ID.
   *
   * @return entry ID
   */
  int GetEntry() const { return m_entry; }

  /**
   * Gets the record timestamp.
   *
   * @return Timestamp, in integer microseconds
   */
  int64_t GetTimestamp() const { return m_timestamp; }

  /**
   * Gets the size of the raw data.
   *
   * @return size
   */
  size_t GetSize() const { return m_data.size(); }

  /**
   * Gets the raw data. Use the GetX functions to decode based on the data type
   * in the entry's start record.
   */
  std::span<const uint8_t> GetRaw() const { return m_data; }

  /**
   * Returns true if the record is a control record.
   *
   * @return True if control record, false if normal data record.
   */
  bool IsControl() const { return m_entry == 0; }

  /**
   * Returns true if the record is a start control record. Use GetStartData()
   * to decode the contents.
   *
   * @return True if start control record, false otherwise.
   */
  bool IsStart() const;

  /**
   * Returns true if the record is a finish control record. Use GetFinishEntry()
   * to decode the contents.
   *
   * @return True if finish control record, false otherwise.
   */
  bool IsFinish() const;

  /**
   * Returns true if the record is a set metadata control record. Use
   * GetSetMetadataData() to decode the contents.
   *
   * @return True if set metadata control record, false otherwise.
   */
  bool IsSetMetadata() const;

  /**
   * Decodes a start control record.
   *
   * @param[out] out start record decoded data (if successful)
   * @return True on success, false on error
   */
  bool GetStartData(StartRecordData* out) const;

  /**
   * Decodes a finish control record.
   *
   * @param[out] out finish record entry ID (if successful)
   * @return True on success, false on error
   */
  bool GetFinishEntry(int* out) const;

  /**
   * Decodes a set metadata control record.
   *
   * @param[out] out set metadata record decoded data (if successful)
   * @return True on success, false on error
   */
  bool GetSetMetadataData(MetadataRecordData* out) const;

  /**
   * Decodes a data record as a boolean. Note if the data type (as indicated in
   * the corresponding start control record for this entry) is not "boolean",
   * invalid results may be returned.
   *
   * @param[out] value boolean value (if successful)
   * @return True on success, false on error
   */
  bool GetBoolean(bool* value) const;

  /**
   * Decodes a data record as an integer. Note if the data type (as indicated in
   * the corresponding start control record for this entry) is not "int64",
   * invalid results may be returned.
   *
   * @param[out] value integer value (if successful)
   * @return True on success, false on error
   */
  bool GetInteger(int64_t* value) const;

  /**
   * Decodes a data record as a float. Note if the data type (as indicated in
   * the corresponding start control record for this entry) is not "float",
   * invalid results may be returned.
   *
   * @param[out] value float value (if successful)
   * @return True on success, false on error
   */
  bool GetFloat(float* value) const;

  /**
   * Decodes a data record as a double. Note if the data type (as indicated in
   * the corresponding start control record for this entry) is not "double",
   * invalid results may be returned.
   *
   * @param[out] value double value (if successful)
   * @return True on success, false on error
   */
  bool GetDouble(double* value) const;

  /**
   * Decodes a data record as a string. Note if the data type (as indicated in
   * the corresponding start control record for this entry) is not "string",
   * invalid results may be returned.
   *
   * @param[out] value string value
   * @return True (never fails)
   */
  bool GetString(std::string_view* value) const;

  /**
   * Decodes a data record as a boolean array. Note if the data type (as
   * indicated in the corresponding start control record for this entry) is not
   * "boolean[]", invalid results may be returned.
   *
   * @param[out] arr boolean array
   * @return True (never fails)
   */
  bool GetBooleanArray(std::vector<int>* arr) const;

  /**
   * Decodes a data record as an integer array. Note if the data type (as
   * indicated in the corresponding start control record for this entry) is not
   * "int64[]", invalid results may be returned.
   *
   * @param[out] arr integer array (if successful)
   * @return True on success, false on error
   */
  bool GetIntegerArray(std::vector<int64_t>* arr) const;

  /**
   * Decodes a data record as a float array. Note if the data type (as
   * indicated in the corresponding start control record for this entry) is not
   * "float[]", invalid results may be returned.
   *
   * @param[out] arr float array (if successful)
   * @return True on success, false on error
   */
  bool GetFloatArray(std::vector<float>* arr) const;

  /**
   * Decodes a data record as a double array. Note if the data type (as
   * indicated in the corresponding start control record for this entry) is not
   * "double[]", invalid results may be returned.
   *
   * @param[out] arr double array (if successful)
   * @return True on success, false on error
   */
  bool GetDoubleArray(std::vector<double>* arr) const;

  /**
   * Decodes a data record as a string array. Note if the data type (as
   * indicated in the corresponding start control record for this entry) is not
   * "string[]", invalid results may be returned.
   *
   * @param[out] arr string array (if successful)
   * @return True on success, false on error
   */
  bool GetStringArray(std::vector<std::string_view>* arr) const;

 private:
  int64_t m_timestamp{0};
  std::span<const uint8_t> m_data;
  int m_entry{-1};
};

class DataLogReader;

/** DataLogReader iterator. */
class DataLogIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = DataLogRecord;
  using pointer = const value_type*;
  using reference = const value_type&;

  DataLogIterator(const DataLogReader* reader, size_t pos)
      : m_reader{reader}, m_pos{pos} {}

  bool operator==(const DataLogIterator& oth) const {
    return m_reader == oth.m_reader && m_pos == oth.m_pos;
  }
  bool operator!=(const DataLogIterator& oth) const {
    return !this->operator==(oth);
  }

  bool operator<(const DataLogIterator& oth) const { return m_pos < oth.m_pos; }
  bool operator>(const DataLogIterator& oth) const {
    return !this->operator<(oth) && !this->operator==(oth);
  }
  bool operator<=(const DataLogIterator& oth) const {
    return !this->operator>(oth);
  }
  bool operator>=(const DataLogIterator& oth) const {
    return !this->operator<(oth);
  }

  DataLogIterator& operator++();

  DataLogIterator operator++(int) {
    DataLogIterator tmp = *this;
    ++*this;
    return tmp;
  }

  reference operator*() const;

  pointer operator->() const { return &this->operator*(); }

 protected:
  const DataLogReader* m_reader;
  size_t m_pos;
  mutable bool m_valid = false;
  mutable DataLogRecord m_value;
};

/** Data log reader (reads logs written by the DataLog class). */
class DataLogReader {
  friend class DataLogIterator;

 public:
  using iterator = DataLogIterator;

  /** Constructs from a memory buffer. */
  explicit DataLogReader(std::unique_ptr<MemoryBuffer> buffer);

  /** Returns true if the data log is valid (e.g. has a valid header). */
  explicit operator bool() const { return IsValid(); }

  /** Returns true if the data log is valid (e.g. has a valid header). */
  bool IsValid() const;

  /**
   * Gets the data log version. Returns 0 if data log is invalid.
   *
   * @return Version number; most significant byte is major, least significant
   *         is minor (so version 1.0 will be 0x0100)
   */
  uint16_t GetVersion() const;

  /**
   * Gets the extra header data.
   *
   * @return Extra header data
   */
  std::string_view GetExtraHeader() const;

  /**
   * Gets the buffer identifier, typically the filename.
   *
   * @return Identifier string
   */
  std::string_view GetBufferIdentifier() const {
    return m_buf ? m_buf->GetBufferIdentifier() : "Invalid";
  }

  /** Returns iterator to first record. */
  iterator begin() const;

  /** Returns end iterator. */
  iterator end() const { return DataLogIterator{this, SIZE_MAX}; }

 private:
  std::unique_ptr<MemoryBuffer> m_buf;

  bool GetRecord(size_t* pos, DataLogRecord* out) const;
  bool GetNextRecord(size_t* pos) const;
};

inline DataLogIterator& DataLogIterator::operator++() {
  if (!m_reader->GetNextRecord(&m_pos)) {
    m_pos = SIZE_MAX;
  }
  m_valid = false;
  return *this;
}

inline DataLogIterator::reference DataLogIterator::operator*() const {
  if (!m_valid) {
    size_t pos = m_pos;
    if (m_reader->GetRecord(&pos, &m_value)) {
      m_valid = true;
    }
  }
  return m_value;
}

}  // namespace wpi::log

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/DataLog.h"

#include <sys/types.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>  // NOLINT(build/include_order)

#include <memoryapi.h>

#else  // _WIN32

#include <sys/mman.h>
#include <unistd.h>

#endif  // _WIN32

#ifdef _MSC_VER
#include <io.h>
#endif

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <system_error>

#include <wpi/Endian.h>
#include <wpi/FileSystem.h>
#include <wpi/MathExtras.h>
#ifdef _WIN32
#include <wpi/WindowsError.h>
#endif
#include <wpi/json.h>
#include <wpi/raw_istream.h>

/*
 * ===DATA STORAGE FORMAT===
 *
 * The overall file format is structured on a 4 KiB basic block size to
 * facilitate memory mapping (to match the typical page size).
 *
 * One or more data "streams" can be stored.  Each data stream is stored
 * as a doubly linked list of data blocks and does not need to be
 * contiguous in the file.
 *
 * Primary data streams are made up of fixed size contiguous records.
 * Individual records cannot span data block boundaries.  Each record consists
 * of a timestamp and one or more data fields.  Data fields may be either fixed
 * or variable size; fixed size data is stored as part of the record, while
 * variable size data is stored in a separate secondary data stream (and a
 * pointer is stored in the record).
 *
 * ===FILE HEADER===
 *
 * The file header is a fixed 4 KiB in size, and contains 0-padded JSON data in
 * the following format.  Additional data can be added to the "custom" field
 * (as long as the overall header fits in 4 KiB).
 *
 * {
 *  "format": <string>,
 *  "version": <integer>,
 *  "directory": <integer>,
 *  "timeResolution": <string>,
 *  "timeEpoch": <string>,
 *  "custom": {...}
 * }
 *
 * format: always "wpilog"
 *
 * version: always 0 for this version of the data format
 *
 * directory: file offset of block containing directory
 *
 * timeResolution: timestamp resolution, typically "us" (integer microseconds)
 *
 * timeEpoch: timestamp epoch in ISO 8601 format.  May be unspecified/empty to
 * indicate the epoch is unknown.
 *
 * custom: custom data, unspecified contents
 *
 * ===DATA BLOCK===
 *
 * Each data block is a multiple of 4 KiB in size and starts with the
 * following data block header:
 * - Data stream ID: 32-bit
 * - File offset of previous block for this stream: 32-bit (0 if first block)
 * - File offset of next block for this stream: 32-bit (0 if last block)
 * - Size of this block in bytes: 32-bit
 * - Number of bytes used in this block: 32-bit
 * - Spare: 32-bit
 *
 * ===DIRECTORY===
 *
 * The directory is a data block that contains 0-padded JSON data in the
 * following format.  The directory is always a single contiguous block of
 * data.  The only field used in the directory data block is the block size and
 * number of bytes used; the other block header fields are unused and set to 0.
 *
 * {
 *  "streams": [
 *   {
 *    "name": <string>,
 *    "id": <integer>,
 *    "recordSize": <integer>,
 *    "firstBlock": <integer>,
 *    "fields" : [
 *     {
 *      "name": <string>
 *      "dataType": <string>,
 *      "size": <integer>,
 *      "fixedSize": <boolean>,
 *      "gapData": <string>,
 *      "varId": <integer>,
 *      "varFirstBlock": <integer>
 *     },
 *     ...
 *    ]
 *   },
 *   ...
 *  ]
 * }
 *
 * ==Streams==
 *
 * A list of primary stream definitions.
 *
 * name: user-defined name for the primary stream.  Must be unique.
 *
 * id: stream numeric identifer for the primary data stream.  Used in data
 * block headers.
 *
 * recordSize: the size of each record (including timestamp) in the primary
 * data stream, in bytes.
 *
 * firstBlock: file offset of the first data block for the primary data
 * stream.  0 if there are no saved data blocks.
 *
 * fields: A list of the data fields in the stream.
 *
 * ==Fields==
 *
 * Each data field may be either fixed or variable size, as indicated by
 * fixedSize.
 *
 * name: user-defined name for the field.  Recommended but not required to
 * be unique.  May also be blank.
 *
 * dataType: standard or user-defined string.  When user-defined, recommend
 * making this java-style (com.foo.bar) or a MIME type.  Some "standard" data
 * types are:
 *  "string"    - UTF-8 string
 *  "boolean"   - 1-byte boolean
 *  "float"     - 32-bit IEEE-758 floating point
 *  "double"    - 64-bit IEEE-758 floating point
 *  "byte[]"    - array of bytes
 *  "boolean[]" - array of 1-byte boolean
 *  "float[]"   - array of 32-bit IEEE-758 floating point
 *  "double[]"  - array of 64-bit IEEE-758 floating point
 *
 * size: the size of the field contents in the primary data stream, in bytes.
 * For a variable size field, this is always 8 (32-bit offset and 32-bit size).
 *
 * fixedSize: true if the field is fixed size, false if the field is variable
 * size.
 *
 * gapData: user-defined string that contains the data that should be written
 * after the variable-sized data in the secondary data stream.  Unused if
 * fixedSize is true.
 *
 * varId: stream numeric identifer for the secondary data stream.  Used in
 * data block headers.  0 if fixedSize is true.
 *
 * varFirstBlock: file offset of the first data block for the secondary data
 * stream.  0 if there are no saved data blocks, or if fixedSize is true.
 *
 * ===PRIMARY STREAM STORAGE===
 *
 * Each record in the primary data stream starts with a 64-bit timestamp.
 * The epoch and resolution of the timestamp is specified by the file header.
 * The timestamps must be monotonically increasing for the find function to
 * work.
 *
 * When there is more than a single field in the stream, the timestamp is
 * followed by a bit array with one bit for each field.  The least significant
 * bit of the first byte is for the first field.  Each bit is 1 if the field
 * contents are valid, 0 otherwise.  This bit array takes ceil(# fields / 8)
 * bytes of storage.
 *
 * The remainder of the record is packed storage of each field in the order
 * listed in the directory.
 *
 * If fixedSize=true for a field, the value stored in the record contains the
 * field data.
 *
 * If fixedSize=false for a field, the value stored in the record contains the
 * offset and size (in that order) of the data contents in the secondary data
 * stream.  The offset and size are both 32-bit.
 *
 * ===SECONDARY STREAM STORAGE===
 *
 * Contains continuous data contents, potentially with data appended after each
 * record (as configured by gapData).  The data contents for a value and its
 * appended data cannot span block boundaries.
 *
 */

using namespace wpi::log;

static constexpr size_t kLargePointerRecordSize =
    DataLogImpl::kTimestampSize + 8 * 2;
static constexpr size_t kSmallPointerRecordSize =
    DataLogImpl::kTimestampSize + 4 * 2;

static uint64_t GetFileSize(int fd) {
#ifdef _WIN32
  uint64_t size = ::_lseeki64(fd, 0, SEEK_END);
  ::_lseeki64(fd, 0, SEEK_SET);
#else
  uint64_t size = ::lseek(fd, 0, SEEK_END);
  ::lseek(fd, 0, SEEK_SET);
#endif
  return size;
}

MappedFile::MappedFile(int fd, size_t length, uint64_t offset, bool readOnly,
                       std::error_code& ec)
    : m_size(length) {
#ifdef _WIN32
  HANDLE origFileHandle = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
  if (origFileHandle == INVALID_HANDLE_VALUE) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return;
  }

  HANDLE fileMappingHandle = ::CreateFileMappingW(
      origFileHandle, 0, readOnly ? PAGE_READONLY : PAGE_READWRITE,
      Hi_32(length), Lo_32(length), 0);
  if (fileMappingHandle == nullptr) {
    ec = wpi::mapWindowsError(GetLastError());
    return;
  }

  m_mapping = ::MapViewOfFile(fileMappingHandle, FILE_MAP_WRITE, offset >> 32,
                              offset & 0xffffffff, length);
  if (m_mapping == nullptr) {
    ec = wpi::mapWindowsError(GetLastError());
    ::CloseHandle(fileMappingHandle);
    return;
  }

  // Close the file mapping handle, as it's kept alive by the file mapping. But
  // neither the file mapping nor the file mapping handle keep the file handle
  // alive, so we need to keep a reference to the file in case all other handles
  // are closed and the file is deleted, which may cause invalid data to be read
  // from the file.
  ::CloseHandle(fileMappingHandle);
  if (!::DuplicateHandle(::GetCurrentProcess(), origFileHandle,
                         ::GetCurrentProcess(), &m_fileHandle, 0, 0,
                         DUPLICATE_SAME_ACCESS)) {
    ec = wpi::mapWindowsError(GetLastError());
    ::UnmapViewOfFile(m_mapping);
    m_mapping = nullptr;
    return;
  }
#else
  m_mapping =
      ::mmap(nullptr, length, readOnly ? PROT_READ : (PROT_READ | PROT_WRITE),
             MAP_SHARED, fd, offset);
  if (m_mapping == MAP_FAILED) {
    ec = std::error_code(errno, std::generic_category());
    m_mapping = nullptr;
  }
#endif
}

void MappedFile::Flush() {
#ifdef _WIN32
  ::FlushViewOfFile(m_mapping, 0);
  ::FlushFileBuffers(m_fileHandle);
#else
  ::msync(m_mapping, m_size, MS_ASYNC);
#endif
}

void MappedFile::Unmap() {
  if (!m_mapping) {
    return;
  }
#ifdef _WIN32
  ::UnmapViewOfFile(m_mapping);
  ::CloseHandle(m_fileHandle);
#else
  ::munmap(m_mapping, m_size);
#endif
  m_mapping = nullptr;
}

std::pair<uint64_t, wpi::ArrayRef<uint8_t>> DataLogImpl::ReadRaw(
    size_t n) const {
  auto raw = m_time.Read(kHeaderSize + n * m_recordSize, m_recordSize);
  if (raw.size() < m_recordSize || raw.size() < kTimestampSize) {
    return {
      0, {}
    }
  };
  uint64_t ts = wpi::support::endian::read64le(raw.data());
  if (m_fixedSize) {
    return {ts, raw.slice(kTimestampSize)};
  } else if (m_recordSize == kLargePointerRecordSize) {
    // use data offset and size to read data file
    return {ts, m_data.Read(
                    wpi::support::endian::read64le(raw.data() + kTimestampSize),
                    wpi::support::endian::read64le(raw.data() + kTimestampSize +
                                                   8))};
  } else {
    // use data offset and size to read data file
    return {ts, m_data.Read(
                    wpi::support::endian::read32le(raw.data() + kTimestampSize),
                    wpi::support::endian::read32le(raw.data() + kTimestampSize +
                                                   4))};
  }
}

void DataLogImpl::Flush() {
  WriteHeader();
  if (m_time.map && !m_time.readOnly) {
    m_time.map.Flush();
  }
  if (m_data.map && !m_data.readOnly) {
    m_data.map.Flush();
  }
}

DataLogImpl::DataLogImpl() {}

DataLogImpl::~DataLogImpl() { WriteHeader(); }

bool DataLogImpl::AppendRaw(uint64_t timestamp, wpi::ArrayRef<uint8_t> data) {
  size_t size = data.size();
  uint8_t* out = AppendRawStart(timestamp, size);
  if (!out) {
    return false;
  }
  std::memcpy(out, data.data(), size);
  AppendRawFinish(size);
  return true;
}

uint8_t* DataLogImpl::AppendRawStart(uint64_t timestamp, uint64_t size) {
  // check monotonic (if checking enabled)
  if (m_checkMonotonic && timestamp <= m_lastTimestamp) {
    return nullptr;
  }

  // check read-only
  if (m_time.readOnly) {
    return nullptr;
  }

  std::error_code ec;
  size_t off = m_time.GetMappedOffset(m_time.writePos, m_recordSize, ec);
  if (ec) {
    return nullptr;
  }

  // write timestamp
  uint8_t* timeBuf = m_time.map.data() + off;
  wpi::support::endian::write64le(timeBuf, timestamp);

  if (m_fixedSize) {
    m_lastTimestamp = timestamp;
    return timeBuf + kTimestampSize;
  }

  // write data to data file
  if (m_recordSize == kLargePointerRecordSize) {
    wpi::support::endian::write64le(timeBuf + kTimestampSize, m_data.writePos);
    wpi::support::endian::write64le(timeBuf + kTimestampSize + 8, size);
  } else {
    wpi::support::endian::write32le(timeBuf + kTimestampSize, m_data.writePos);
    wpi::support::endian::write32le(timeBuf + kTimestampSize + 4, size);
  }

  off = m_data.GetMappedOffset(m_data.writePos, size, ec);
  if (ec) {
    return nullptr;
  }
  m_lastTimestamp = timestamp;
  return m_data.map.data() + off;
}

void DataLogImpl::AppendRawFinish(uint64_t size) {
  if (!m_fixedSize) {
    m_data.writePos += size;

    // write gap data (if any)
    if (!m_gapData.empty()) {
      m_data.Write(
          m_data.writePos,
          wpi::makeArrayRef(reinterpret_cast<const uint8_t*>(m_gapData.data()),
                            m_gapData.size()));
      m_data.writePos += m_gapData.size();
    }
  }

  m_time.writePos += m_recordSize;

  // periodic flush (if enabled)
  if (m_periodicFlush != 0 && ++m_periodicFlushCount >= m_periodicFlush) {
    Flush();
    m_periodicFlushCount = 0;
  }
}

size_t DataLogImpl::FindImpl(uint64_t timestamp, size_t first,
                             size_t last) const {
  size_t it = 0;
  size_t count = (std::min)(size(), last) - first;
  while (count > 0) {
    it = first;
    size_t step = count / 2;
    it += step;
    if (ReadRaw(it).first < timestamp) {
      first = ++it;
      count -= step + 1;
    } else {
      count = step;
    }
  }
  return first;
}

wpi::Error DataLogImpl::Check(const wpi::Twine& dataType,
                              const wpi::Twine& dataLayout,
                              unsigned int recordSize, bool checkType,
                              bool checkLayout, bool checkSize) const {
  wpi::SmallString<128> dataTypeBuf;
  wpi::SmallString<128> dataLayoutBuf;
  if ((checkType && m_dataType != dataType.toStringRef(dataTypeBuf)) ||
      (checkLayout && m_dataLayout != dataLayout.toStringRef(dataLayoutBuf)) ||
      (checkSize &&
       ((recordSize != 0 && (!m_fixedSize || m_recordSize != recordSize)) ||
        (recordSize == 0 &&
         (m_fixedSize || (m_recordSize != kLargePointerRecordSize &&
                          m_recordSize != kSmallPointerRecordSize)))))) {
    return wpi::errorCodeToError(
        std::make_error_code(std::errc::wrong_protocol_type));
  } else {
    return wpi::Error::success();
  }
}

wpi::Error DataLogImpl::DoOpen(const wpi::Twine& filename,
                               const wpi::Twine& dataType,
                               const wpi::Twine& dataLayout,
                               unsigned int recordSize,
                               CreationDisposition disp, const Config& config) {
  // open the time file
  std::error_code ec = m_time.Open(filename, disp, config.readOnly);
  if (ec) {
    return wpi::errorCodeToError(ec);
  }
  m_time.fileSize = GetFileSize(m_time.fd);

  if (disp == CD_OpenExisting ||
      (disp == CD_OpenAlways && m_time.fileSize > 0)) {
    if (wpi::Error e = ReadHeader()) {
      return e;
    }

    // check configuration
    if (wpi::Error e = Check(dataType, dataLayout, recordSize, config.checkType,
                             config.checkLayout, config.checkSize)) {
      return e;
    }
  } else {
    m_dataType = dataType.str();
    m_dataLayout = dataLayout.str();
    m_fixedSize = recordSize != 0;
    m_recordSize = m_fixedSize ? recordSize
                               : (config.largeData ? kLargePointerRecordSize
                                                   : kSmallPointerRecordSize);
    m_gapData = config.gapData;
    m_time.writePos = kHeaderSize;
  }

  // set configuration
  m_periodicFlush = config.periodicFlush;
  m_checkMonotonic = config.checkMonotonic;

  m_time.maxGrowSize = config.maxGrowSize * m_recordSize;
  m_time.mapGrowSize = config.initialSize * m_recordSize;
  m_time.maxMapSize = config.maxMapSize;

  m_data.maxGrowSize = config.maxDataGrowSize;
  m_data.mapGrowSize = config.initialDataSize;
  m_data.maxMapSize = config.maxMapSize;

  // set up the time file for writing
  if (m_time.writePos > (kHeaderSize + m_recordSize)) {
    // Read last timestamp
    size_t off = m_time.GetMappedOffset(m_time.writePos - m_recordSize,
                                        m_recordSize * 2, ec);
    if (ec) {
      return wpi::errorCodeToError(ec);
    }
    m_lastTimestamp =
        wpi::support::endian::read64le(m_time.map.const_data() + off);
  } else {
    m_time.GetMappedOffset(m_time.writePos, m_recordSize, ec);
    if (ec) {
      return wpi::errorCodeToError(ec);
    }
  }

  if (!m_fixedSize) {
    // open the data file
    ec = m_data.Open(filename + ".data", disp, config.readOnly);
    if (ec) {
      return wpi::errorCodeToError(ec);
    }
    m_data.fileSize = GetFileSize(m_data.fd);
    m_data.GetMappedOffset(m_data.writePos, 1024, ec);
    if (ec) {
      return wpi::errorCodeToError(ec);
    }
  }

  return wpi::Error::success();
}

wpi::Error DataLogImpl::ReadHeader() {
  // don't use memory map for this; it doesn't exist yet
  wpi::raw_fd_istream is(m_time.fd, false);
  wpi::json j;
  try {
    j = wpi::json::parse(is);
  } catch (const wpi::json::parse_error&) {
    return wpi::errorCodeToError(
        std::make_error_code(std::errc::wrong_protocol_type));
  }
  if (!j.is_object()) {
    return wpi::errorCodeToError(
        std::make_error_code(std::errc::wrong_protocol_type));
  }
  try {
    m_dataType = j.at("dataType").get<std::string>();
    m_dataLayout = j.at("dataLayout").get<std::string>();
    m_recordSize = j.at("recordSize").get<unsigned int>();
    m_fixedSize = j.at("fixedSize").get<bool>();
    m_gapData = j.at("gapData").get<std::string>();
    m_time.writePos = j.at("timeWritePos").get<uint64_t>();
    m_data.writePos = j.at("dataWritePos").get<uint64_t>();
  } catch (const wpi::json::exception&) {
    return wpi::errorCodeToError(
        std::make_error_code(std::errc::wrong_protocol_type));
  }
  return wpi::Error::success();
}

void DataLogImpl::WriteHeader() {
  if (!m_time.map || m_time.readOnly) {
    return;
  }
  wpi::json j = {{"dataType", m_dataType},
                 {"dataLayout", m_dataLayout},
                 {"recordSize", m_recordSize},
                 {"fixedSize", m_fixedSize},
                 {"gapData", m_gapData},
                 {"timeWritePos", m_time.mapOffset + m_time.writePos},
                 {"dataWritePos", m_data.mapOffset + m_data.writePos}};
  std::vector<uint8_t> buf;
  wpi::raw_uvector_ostream os(buf);
  j.dump(os, 1);
  os << '\n';
  buf.resize(kHeaderSize);
  m_time.Write(0, buf);
}

std::error_code DataLogImpl::FileInfo::Open(const wpi::Twine& filename,
                                            CreationDisposition disp, bool ro) {
  readOnly = ro;
  return wpi::sys::fs::openFile(
      filename, fd, static_cast<wpi::sys::fs::CreationDisposition>(disp),
      ro ? wpi::sys::fs::FA_Read
         : (wpi::sys::fs::FA_Read | wpi::sys::fs::FA_Write),
      wpi::sys::fs::OF_None);
}

void DataLogImpl::FileInfo::Close() {
  map.Unmap();
  if (fd != -1 && writePos != 0 && !readOnly) {
#ifdef _WIN32
    _chsize_s(fd, writePos);
#else
    if (::ftruncate(fd, writePos) == -1) {
      std::perror("could not truncate during close");
    }
#endif
  }
  if (fd != -1) {
    ::close(fd);
  }
  fd = -1;
}

size_t DataLogImpl::FileInfo::GetMappedOffset(uint64_t pos, size_t len,
                                              std::error_code& ec) const {
  // TODO: handle smaller mapping window (instead of entire file)

  // easy case: already in mapped window
  if (map && pos >= mapOffset && (pos + len - mapOffset) <= map.size()) {
    return pos - mapOffset;
  }

  if (!readOnly) {
    // round up to multiple of mapGrowSize
    uint64_t size = ((pos + len + mapGrowSize - 1) / mapGrowSize) * mapGrowSize;
    if (size > fileSize) {
      fileSize = size;
    }

    // scale up mapGrowSize until it gets to maxGrowSize
    if (mapGrowSize < maxGrowSize) {
      mapGrowSize *= 2;
      if (mapGrowSize > maxGrowSize) {
        mapGrowSize = maxGrowSize;
      }
    }

    // update file size
#ifdef _WIN32
    _chsize_s(fd, fileSize);
#else
    if (::ftruncate(fd, fileSize) == -1) {
      std::perror("could not update file size");
    }
#endif
  }

  // update map
  map.Unmap();
  map = MappedFile(fd, fileSize, 0, readOnly, ec);
  if (ec) {
    return SIZE_MAX;
  }

  return pos - mapOffset;
}

wpi::ArrayRef<uint8_t> DataLogImpl::FileInfo::Read(uint64_t pos,
                                                   size_t len) const {
  std::error_code ec;
  size_t off = GetMappedOffset(pos, len, ec);
  if (ec) {
    return {}
  };
  return wpi::makeArrayRef(map.const_data() + off, len);
}

void DataLogImpl::FileInfo::Write(uint64_t pos, wpi::ArrayRef<uint8_t> data) {
  std::error_code ec;
  size_t off = GetMappedOffset(pos, data.size(), ec);
  if (ec) {
    return;
  }
  std::memcpy(map.data() + off, data.data(), data.size());
}

wpi::Expected<DataLog> DataLog::Open(const wpi::Twine& filename,
                                     const Config& config) {
  Config configCopy = config;
  configCopy.checkType = false;
  configCopy.checkSize = false;
  configCopy.checkLayout = false;
  return Open(filename, "", "", 0, CD_OpenExisting, configCopy);
}

wpi::Expected<DataLog> DataLog::Open(const wpi::Twine& filename,
                                     const wpi::Twine& dataType,
                                     const wpi::Twine& dataLayout,
                                     unsigned int recordSize,
                                     CreationDisposition disp,
                                     const Config& config) {
  DataLog log(new DataLogImpl, true);

  if (wpi::Error e = log.m_impl->DoOpen(filename, dataType, dataLayout,
                                        recordSize, disp, config)) {
    return wpi::Expected<DataLog> { std::move(e) }
  };
  return wpi::Expected<DataLog>{std::move(log)};
}

bool DoubleLog::Append(uint64_t timestamp, double value) {
  uint8_t buf[8];
  wpi::support::endian::write64le(buf, wpi::DoubleToBits(value));
  return m_impl->AppendRaw(timestamp, buf);
}

std::pair<uint64_t, double> DoubleLog::operator[](size_t n) const {
  auto [ts, arr] = m_impl->ReadRaw(n);
  return {ts, wpi::BitsToDouble(wpi::support::endian::read64le(arr.data()))};
}

bool BooleanArrayLog::Append(uint64_t timestamp, wpi::ArrayRef<bool> arr) {
  uint8_t* out = m_impl->AppendRawStart(timestamp, arr.size());
  if (!out) {
    return false;
  }
  for (bool value : arr) {
    *out++ = value ? 1 : 0;
  }
  m_impl->AppendRawFinish(arr.size());
  return true;
}

bool BooleanArrayLog::Append(uint64_t timestamp, wpi::ArrayRef<int> arr) {
  uint8_t* out = m_impl->AppendRawStart(timestamp, arr.size());
  if (!out) {
    return false;
  }
  for (int value : arr) {
    *out++ = value ? 1 : 0;
  }
  m_impl->AppendRawFinish(arr.size());
  return true;
}

std::pair<uint64_t, wpi::ArrayRef<bool>> BooleanArrayLog::Get(
    size_t n, wpi::SmallVectorImpl<bool>& buf) const {
  auto [ts, arr] = m_impl->ReadRaw(n);
  buf.clear();
  buf.reserve(arr.size());
  for (uint8_t value : arr) {
    buf.emplace_back(value != 0);
  }
  return {ts, wpi::makeArrayRef(buf.data(), buf.size())};
}

std::pair<uint64_t, wpi::ArrayRef<int>> BooleanArrayLog::Get(
    size_t n, wpi::SmallVectorImpl<int>& buf) const {
  auto [ts, arr] = m_impl->ReadRaw(n);
  buf.clear();
  buf.reserve(arr.size());
  for (uint8_t value : arr) {
    buf.emplace_back(value != 0 ? 1 : 0);
  }
  return {ts, wpi::makeArrayRef(buf.data(), buf.size())};
}

double DoubleArrayLogArrayProxy::operator[](size_t n) const {
  return wpi::BitsToDouble(
      wpi::support::endian::read64le(m_data.data() + n * 8));
}

bool DoubleArrayLog::Append(uint64_t timestamp, wpi::ArrayRef<double> arr) {
  uint8_t* out = m_impl->AppendRawStart(timestamp, arr.size() * 8);
  if (!out) {
    return false;
  }
  for (double value : arr) {
    wpi::support::endian::write64le(out, wpi::DoubleToBits(value));
    out += 8;
  }
  m_impl->AppendRawFinish(arr.size() * 8);
  return true;
}

std::pair<uint64_t, wpi::ArrayRef<double>> DoubleArrayLog::Get(
    size_t n, wpi::SmallVectorImpl<double>& buf) const {
  auto [ts, arr] = m_impl->ReadRaw(n);
  buf.clear();
  buf.reserve(arr.size() / 8);
  for (size_t i = 0; i < arr.size() / 8; ++i) {
    buf.emplace_back(
        wpi::BitsToDouble(wpi::support::endian::read64le(arr.data() + i * 8)));
  }
  return {ts, wpi::makeArrayRef(buf.data(), buf.size())};
}

wpi::StringRef StringArrayLogArrayProxy::operator[](size_t n) const {
  const uint8_t* ptrRec = m_data.slice(4 + n * 8, 8).data();
  uint32_t off = wpi::support::endian::read32le(ptrRec);
  uint32_t size = wpi::support::endian::read32le(ptrRec + 4);
  return wpi::StringRef(
      reinterpret_cast<const char*>(m_data.slice(off, size).data()), size);
}

bool StringArrayLog::Append(uint64_t timestamp,
                            wpi::ArrayRef<std::string> arr) {
  // calculate size
  uint32_t off = 4 + 8 * arr.size();
  uint32_t size = off;
  for (auto&& value : arr) {
    size += value.size() + 1;
  }

  uint8_t* out = m_impl->AppendRawStart(timestamp, size);
  if (!out) {
    return false;
  }

  // number of strings
  wpi::support::endian::write32le(out, arr.size());
  out += 4;

  // offset, length for each string
  for (auto&& value : arr) {
    wpi::support::endian::write32le(out, off);
    wpi::support::endian::write32le(out, value.size());
    off += value.size() + 1;
    out += 8;
  }

  // string data, null terminate after each string
  for (auto&& value : arr) {
    std::memcpy(out, value.data(), value.size());
    out += value.size();
    *out++ = '\0';
  }

  m_impl->AppendRawFinish(size);
  return true;
}

bool StringArrayLog::Append(uint64_t timestamp,
                            wpi::ArrayRef<wpi::StringRef> arr) {
  // calculate size
  uint32_t off = 4 + 8 * arr.size();
  uint32_t size = off;
  for (auto&& value : arr) {
    size += value.size() + 1;
  }

  uint8_t* out = m_impl->AppendRawStart(timestamp, size);
  if (!out) {
    return false;
  }

  // number of strings
  wpi::support::endian::write32le(out, arr.size());
  out += 4;

  // offset, length for each string
  for (auto&& value : arr) {
    wpi::support::endian::write32le(out, off);
    out += 4;
    wpi::support::endian::write32le(out, value.size());
    out += 4;
    off += value.size() + 1;
  }

  // string data, null terminate after each string
  for (auto&& value : arr) {
    std::memcpy(out, value.data(), value.size());
    out += value.size();
    *out++ = '\0';
  }

  m_impl->AppendRawFinish(size);
  return true;
}

std::pair<uint64_t, wpi::ArrayRef<std::string>> StringArrayLog::Get(
    size_t n, wpi::SmallVectorImpl<std::string>& buf) const {
  auto [ts, arr] = m_impl->ReadRaw(n);

  // number of strings
  uint32_t num = wpi::support::endian::read32le(arr.data());

  buf.clear();
  buf.reserve(num);
  for (uint32_t i = 0; i < num; ++i) {
    const uint8_t* ptrRec = arr.slice(4 + i * 8, 8).data();
    uint32_t off = wpi::support::endian::read32le(ptrRec);
    uint32_t size = wpi::support::endian::read32le(ptrRec + 4);
    buf.emplace_back(reinterpret_cast<const char*>(arr.slice(off, size).data()),
                     size);
  }
  return {ts, wpi::makeArrayRef(buf.data(), buf.size())};
}

std::pair<uint64_t, wpi::ArrayRef<wpi::StringRef>> StringArrayLog::Get(
    size_t n, wpi::SmallVectorImpl<wpi::StringRef>& buf) const {
  auto [ts, arr] = m_impl->ReadRaw(n);

  // number of strings
  uint32_t num = wpi::support::endian::read32le(arr.data());

  buf.clear();
  buf.reserve(num);
  for (uint32_t i = 0; i < num; ++i) {
    const uint8_t* ptrRec = arr.slice(4 + i * 8, 8).data();
    uint32_t off = wpi::support::endian::read32le(ptrRec);
    uint32_t size = wpi::support::endian::read32le(ptrRec + 4);
    buf.emplace_back(reinterpret_cast<const char*>(arr.slice(off, size).data()),
                     size);
  }
  return {ts, wpi::makeArrayRef(buf.data(), buf.size())};
}

std::pair<uint64_t, StringArrayLogArrayProxy> StringArrayLog::operator[](
    size_t n) const {
  auto [ts, arr] = m_impl->ReadRaw(n);
  return {ts, StringArrayLogArrayProxy(
                  wpi::support::endian::read32le(arr.data()), arr)};
}

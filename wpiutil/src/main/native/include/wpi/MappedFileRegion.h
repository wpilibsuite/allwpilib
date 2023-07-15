// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <system_error>

// Duplicated from fs.h to avoid a dependency
namespace fs {
#if defined(_WIN32)
// A Win32 HANDLE is a typedef of void*
using file_t = void*;
#else
using file_t = int;
#endif
}  // namespace fs

namespace wpi {

class MappedFileRegion {
 public:
  enum MapMode {
    kReadOnly,   ///< May only access map via const_data as read only.
    kReadWrite,  ///< May access map via data and modify it. Written to path.
    kPriv        ///< May modify via data, but changes are lost on destruction.
  };

  MappedFileRegion() = default;
  MappedFileRegion(fs::file_t f, uint64_t length, uint64_t offset,
                   MapMode mapMode, std::error_code& ec);
  ~MappedFileRegion() { Unmap(); }

  MappedFileRegion(const MappedFileRegion&) = delete;
  MappedFileRegion& operator=(const MappedFileRegion&) = delete;

  MappedFileRegion(MappedFileRegion&& rhs)
      : m_size(rhs.m_size), m_mapping(rhs.m_mapping) {
    rhs.m_mapping = nullptr;
#ifdef _WIN32
    m_fileHandle = rhs.m_fileHandle;
    rhs.m_fileHandle = nullptr;
#endif
  }

  MappedFileRegion& operator=(MappedFileRegion&& rhs) {
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

  uint64_t size() const { return m_size; }
  uint8_t* data() const { return static_cast<uint8_t*>(m_mapping); }
  const uint8_t* const_data() const {
    return static_cast<const uint8_t*>(m_mapping);
  }

  /**
   * Returns required alignment.
   */
  static size_t GetAlignment();

 private:
  uint64_t m_size = 0;
  void* m_mapping = nullptr;
#ifdef _WIN32
  fs::file_t m_fileHandle = nullptr;
#endif
};

}  // namespace wpi

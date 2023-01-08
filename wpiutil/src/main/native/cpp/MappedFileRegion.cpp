// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/MappedFileRegion.h"

#include <sys/types.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>  // NOLINT(build/include_order)

#include <memoryapi.h>
#include <sysinfoapi.h>

#else  // _WIN32

#include <sys/mman.h>
#include <unistd.h>

#endif  // _WIN32

#ifdef _MSC_VER
#include <io.h>
#endif

#ifdef _WIN32
#include "wpi/WindowsError.h"
#endif

using namespace wpi;

MappedFileRegion::MappedFileRegion(fs::file_t f, uint64_t length,
                                   uint64_t offset, MapMode mapMode,
                                   std::error_code& ec)
    : m_size(length) {
#ifdef _WIN32
  if (f == INVALID_HANDLE_VALUE) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return;
  }

  HANDLE fileMappingHandle = ::CreateFileMappingW(
      f, 0, mapMode == kReadOnly ? PAGE_READONLY : PAGE_READWRITE, length >> 32,
      length & 0xffffffff, 0);
  if (fileMappingHandle == nullptr) {
    ec = wpi::mapWindowsError(GetLastError());
    return;
  }

  DWORD dwDesiredAccess = 0;
  switch (mapMode) {
    case kReadOnly:
      dwDesiredAccess = FILE_MAP_READ;
      break;
    case kReadWrite:
      dwDesiredAccess = FILE_MAP_WRITE;
      break;
    case kPriv:
      dwDesiredAccess = FILE_MAP_WRITE | FILE_MAP_COPY;
      break;
  }
  m_mapping = ::MapViewOfFile(fileMappingHandle, dwDesiredAccess, offset >> 32,
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
  if (!::DuplicateHandle(::GetCurrentProcess(), f, ::GetCurrentProcess(),
                         &m_fileHandle, 0, 0, DUPLICATE_SAME_ACCESS)) {
    ec = wpi::mapWindowsError(GetLastError());
    ::UnmapViewOfFile(m_mapping);
    m_mapping = nullptr;
    return;
  }
#else
  m_mapping =
      ::mmap(nullptr, length,
             mapMode == kReadOnly ? PROT_READ : (PROT_READ | PROT_WRITE),
             mapMode == kPriv ? MAP_PRIVATE : MAP_SHARED, f, offset);
  if (m_mapping == MAP_FAILED) {
    ec = std::error_code(errno, std::generic_category());
    m_mapping = nullptr;
  }
#endif
}

void MappedFileRegion::Flush() {
#ifdef _WIN32
  ::FlushViewOfFile(m_mapping, 0);
  ::FlushFileBuffers(m_fileHandle);
#else
  ::msync(m_mapping, m_size, MS_ASYNC);
#endif
}

void MappedFileRegion::Unmap() {
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

size_t MappedFileRegion::GetAlignment() {
#ifdef _WIN32
  SYSTEM_INFO SysInfo;
  ::GetSystemInfo(&SysInfo);
  return SysInfo.dwAllocationGranularity;
#else
  static long pageSize = ::getpagesize();  // NOLINT
  if (pageSize < 0) {
    pageSize = 4096;
  }
  return pageSize;
#endif
}

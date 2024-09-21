// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//===--- MemoryBuffer.cpp - Memory Buffer implementation ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements the MemoryBuffer interface.
//
//===----------------------------------------------------------------------===//

#include "wpi/MemoryBuffer.h"

#ifdef _MSC_VER
// no matching operator delete
#pragma warning(disable : 4291)
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>  // NOLINT(build/include_order)

#endif

#include <sys/stat.h>
#include <sys/types.h>

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

#include <cassert>
#include <cerrno>
#include <cstring>
#include <new>
#include <system_error>

#include "wpi/Errc.h"
#include "wpi/Errno.h"
#include "wpi/MappedFileRegion.h"
#include "wpi/MathExtras.h"
#include "wpi/SmallVector.h"
#include "wpi/SmallVectorMemoryBuffer.h"
#include "wpi/fs.h"

#ifdef _WIN32
#include "wpi/WindowsError.h"
#endif

using namespace wpi;

//===----------------------------------------------------------------------===//
// MemoryBuffer implementation itself.
//===----------------------------------------------------------------------===//

MemoryBuffer::~MemoryBuffer() {}

/// init - Initialize this MemoryBuffer as a reference to externally allocated
/// memory.
void MemoryBuffer::Init(const uint8_t* bufStart, const uint8_t* bufEnd) {
  m_bufferStart = bufStart;
  m_bufferEnd = bufEnd;
}

//===----------------------------------------------------------------------===//
// MemoryBufferMem implementation.
//===----------------------------------------------------------------------===//

/// CopyStringRef - Copies contents of a StringRef into a block of memory and
/// null-terminates it.
static void CopyStringView(uint8_t* memory, std::string_view data) {
  if (!data.empty()) {
    std::memcpy(memory, data.data(), data.size());
  }
  memory[data.size()] = 0;  // Null terminate string.
}

namespace {
struct NamedBufferAlloc {
  std::string_view name;
  explicit NamedBufferAlloc(std::string_view name) : name(name) {}
};
}  // namespace

void* operator new(size_t N, NamedBufferAlloc alloc) {
  uint8_t* mem = static_cast<uint8_t*>(operator new(N + alloc.name.size() + 1));
  CopyStringView(mem + N, alloc.name);
  return mem;
}

namespace {
/// MemoryBufferMem - Named MemoryBuffer pointing to a block of memory.
template <typename MB>
class MemoryBufferMem : public MB {
 public:
  explicit MemoryBufferMem(std::span<const uint8_t> inputData) {
    MemoryBuffer::Init(inputData.data(), inputData.data() + inputData.size());
  }

  /// Disable sized deallocation for MemoryBufferMem, because it has
  /// tail-allocated data.
  void operator delete(void* p) { ::operator delete(p); }  // NOLINT

  std::string_view GetBufferIdentifier() const override {
    // The name is stored after the class itself.
    return std::string_view(reinterpret_cast<const char*>(this + 1));
  }

  MemoryBuffer::BufferKind GetBufferKind() const override {
    return MemoryBuffer::MemoryBuffer_Malloc;
  }
};
}  // namespace

template <typename MB>
static std::unique_ptr<MB> GetFileAux(std::string_view filename,
                                      std::error_code& ec, int64_t fileSize,
                                      uint64_t mapSize, uint64_t offset);

std::unique_ptr<MemoryBuffer> MemoryBuffer::GetMemBuffer(
    std::span<const uint8_t> inputData, std::string_view bufferName) {
  auto* ret = new (NamedBufferAlloc(bufferName))
      MemoryBufferMem<MemoryBuffer>(inputData);
  return std::unique_ptr<MemoryBuffer>(ret);
}

std::unique_ptr<MemoryBuffer> MemoryBuffer::GetMemBuffer(MemoryBufferRef ref) {
  return std::unique_ptr<MemoryBuffer>(
      GetMemBuffer(ref.GetBuffer(), ref.GetBufferIdentifier()));
}

static std::unique_ptr<WritableMemoryBuffer> GetMemBufferCopyImpl(
    std::span<const uint8_t> inputData, std::string_view bufferName,
    std::error_code& ec) {
  auto buf =
      WritableMemoryBuffer::GetNewUninitMemBuffer(inputData.size(), bufferName);
  if (!buf) {
    ec = make_error_code(errc::not_enough_memory);
    return nullptr;
  }
  std::memcpy(buf->begin(), inputData.data(), inputData.size());
  return buf;
}

std::unique_ptr<MemoryBuffer> MemoryBuffer::GetMemBufferCopy(
    std::span<const uint8_t> inputData, std::string_view bufferName) {
  std::error_code ec;
  return GetMemBufferCopyImpl(inputData, bufferName, ec);
}

std::unique_ptr<MemoryBuffer> MemoryBuffer::GetFileSlice(
    std::string_view filePath, std::error_code& ec, uint64_t mapSize,
    uint64_t offset) {
  return GetFileAux<MemoryBuffer>(filePath, ec, -1, mapSize, offset);
}

//===----------------------------------------------------------------------===//
// MemoryBuffer::getFile implementation.
//===----------------------------------------------------------------------===//

namespace {

template <typename MB>
constexpr auto kMapMode = MappedFileRegion::kReadOnly;
template <>
constexpr auto kMapMode<MemoryBuffer> = MappedFileRegion::kReadOnly;
template <>
constexpr auto kMapMode<WritableMemoryBuffer> = MappedFileRegion::kPriv;
template <>
constexpr auto kMapMode<WriteThroughMemoryBuffer> =
    MappedFileRegion::kReadWrite;

/// Memory maps a file descriptor using MappedFileRegion.
///
/// This handles converting the offset into a legal offset on the platform.
template <typename MB>
class MemoryBufferMMapFile : public MB {
  MappedFileRegion m_mfr;

  static uint64_t getLegalMapOffset(uint64_t offset) {
    return offset & ~(MappedFileRegion::GetAlignment() - 1);
  }

  static uint64_t getLegalMapSize(uint64_t len, uint64_t offset) {
    return len + (offset - getLegalMapOffset(offset));
  }

  const uint8_t* getStart(uint64_t len, uint64_t offset) {
    return m_mfr.const_data() + (offset - getLegalMapOffset(offset));
  }

 public:
  MemoryBufferMMapFile(fs::file_t f, uint64_t len, uint64_t offset,
                       std::error_code& ec)
      : m_mfr(f, getLegalMapSize(len, offset), getLegalMapOffset(offset),
              kMapMode<MB>, ec) {
    if (!ec) {
      const uint8_t* Start = getStart(len, offset);
      MemoryBuffer::Init(Start, Start + len);
    }
  }

  /// Disable sized deallocation for MemoryBufferMMapFile, because it has
  /// tail-allocated data.
  void operator delete(void* p) { ::operator delete(p); }  // NOLINT

  std::string_view GetBufferIdentifier() const override {
    // The name is stored after the class itself.
    return std::string_view(reinterpret_cast<const char*>(this + 1));
  }

  MemoryBuffer::BufferKind GetBufferKind() const override {
    return MemoryBuffer::MemoryBuffer_MMap;
  }
};
}  // namespace

static std::unique_ptr<WritableMemoryBuffer> GetMemoryBufferForStream(
    fs::file_t f, std::string_view bufferName, std::error_code& ec) {
  constexpr size_t ChunkSize = 4096 * 4;
  SmallVector<uint8_t, ChunkSize> buffer;
  uint64_t size = 0;
#ifdef _WIN32
  DWORD readBytes;
#else
  ssize_t readBytes;
#endif
  // Read into Buffer until we hit EOF.
  do {
    buffer.resize_for_overwrite(size + ChunkSize);
#ifdef _WIN32
    if (!ReadFile(f, buffer.begin() + size, ChunkSize, &readBytes, nullptr)) {
      ec = mapWindowsError(GetLastError());
      return nullptr;
    }
#else
    readBytes = sys::RetryAfterSignal(-1, ::read, f, buffer.begin() + size,
                                      ChunkSize);
    if (readBytes == -1) {
      ec = std::error_code(errno, std::generic_category());
      return nullptr;
    }
#endif
    size += readBytes;
  } while (readBytes != 0);
  buffer.truncate(size);

  return GetMemBufferCopyImpl(buffer, bufferName, ec);
}

wpi::expected<std::unique_ptr<MemoryBuffer>, std::error_code>
MemoryBuffer::GetFile(std::string_view filename, int64_t fileSize) {
  std::error_code ec;
  auto ret = GetFileAux<MemoryBuffer>(filename, ec, fileSize, fileSize, 0);
  if (ec) {
    return wpi::unexpected{ec};
  }
  return ret;
}

template <typename MB>
static std::unique_ptr<MB> GetOpenFileImpl(fs::file_t f,
                                           std::string_view filename,
                                           std::error_code& ec,
                                           uint64_t fileSize, uint64_t mapSize,
                                           int64_t offset);

template <typename MB>
static std::unique_ptr<MB> GetFileAux(std::string_view filename,
                                      std::error_code& ec, int64_t fileSize,
                                      uint64_t mapSize, uint64_t offset) {
  fs::file_t F = fs::OpenFileForRead(filename, ec, fs::OF_None);
  if (ec) {
    return nullptr;
  }

  auto Ret = GetOpenFileImpl<MB>(F, filename, ec, fileSize, mapSize, offset);
  fs::CloseFile(F);
  return Ret;
}

std::unique_ptr<WritableMemoryBuffer> WritableMemoryBuffer::GetFile(
    std::string_view filename, std::error_code& ec, int64_t fileSize) {
  return GetFileAux<WritableMemoryBuffer>(filename, ec, fileSize, fileSize, 0);
}

std::unique_ptr<WritableMemoryBuffer> WritableMemoryBuffer::GetFileSlice(
    std::string_view filename, std::error_code& ec, uint64_t mapSize,
    uint64_t offset) {
  return GetFileAux<WritableMemoryBuffer>(filename, ec, -1, mapSize, offset);
}

std::unique_ptr<WritableMemoryBuffer>
WritableMemoryBuffer::GetNewUninitMemBuffer(size_t size,
                                            std::string_view bufferName) {
  using MemBuffer = MemoryBufferMem<WritableMemoryBuffer>;
  // Allocate space for the MemoryBuffer, the data and the name. It is important
  // that MemoryBuffer and data are aligned so PointerIntPair works with them.
  // TODO: Is 16-byte alignment enough?  We copy small object files with large
  // alignment expectations into this buffer.
  size_t alignedStringLen =
      alignTo(sizeof(MemBuffer) + bufferName.size() + 1, 16);
  size_t realLen = alignedStringLen + size + 1;
  uint8_t* mem = static_cast<uint8_t*>(operator new(realLen, std::nothrow));
  if (!mem) {
    return nullptr;
  }

  // The name is stored after the class itself.
  CopyStringView(mem + sizeof(MemBuffer), bufferName);

  // The buffer begins after the name and must be aligned.
  uint8_t* buf = mem + alignedStringLen;
  buf[size] = 0;  // Null terminate buffer.

  auto* ret = new (mem) MemBuffer({buf, size});
  return std::unique_ptr<WritableMemoryBuffer>(ret);
}

std::unique_ptr<WritableMemoryBuffer> WritableMemoryBuffer::GetNewMemBuffer(
    size_t size, std::string_view bufferName) {
  auto sb = WritableMemoryBuffer::GetNewUninitMemBuffer(size, bufferName);
  if (!sb) {
    return nullptr;
  }
  std::memset(sb->begin(), 0, size);
  return sb;
}

static std::unique_ptr<WriteThroughMemoryBuffer> GetReadWriteFile(
    std::string_view filename, std::error_code& ec, uint64_t fileSize,
    uint64_t mapSize, uint64_t offset) {
  fs::file_t f =
      fs::OpenFileForReadWrite(filename, ec, fs::CD_OpenExisting, fs::OF_None);
  if (ec) {
    return nullptr;
  }

  // Default is to map the full file.
  if (mapSize == uint64_t(-1)) {
    // If we don't know the file size, use fstat to find out.  fstat on an open
    // file descriptor is cheaper than stat on a random path.
    if (fileSize == uint64_t(-1)) {
#ifdef _WIN32
      // If this not a file or a block device (e.g. it's a named pipe
      // or character device), we can't mmap it, so error out.
      if (GetFileType(f) != FILE_TYPE_DISK) {
        ec = std::error_code(errno, std::generic_category());
        return nullptr;
      }

      LARGE_INTEGER fileSizeWin;
      if (!GetFileSizeEx(f, &fileSizeWin)) {
        ec = wpi::mapWindowsError(GetLastError());
        return nullptr;
      }
      fileSize = fileSizeWin.QuadPart;
#else
      struct stat status;
      if (fstat(f, &status) < 0) {
        ec = std::error_code(errno, std::generic_category());
        return nullptr;
      }

      // If this not a file or a block device (e.g. it's a named pipe
      // or character device), we can't mmap it, so error out.
      if (!S_ISREG(status.st_mode) && !S_ISBLK(status.st_mode)) {
        ec = make_error_code(errc::invalid_argument);
        return nullptr;
      }

      fileSize = status.st_size;
#endif
    }
    mapSize = fileSize;
  }

  std::unique_ptr<WriteThroughMemoryBuffer> result(new (NamedBufferAlloc(
      filename)) MemoryBufferMMapFile<WriteThroughMemoryBuffer>(f, mapSize,
                                                                offset, ec));
  if (ec) {
    return nullptr;
  }
  return result;
}

std::unique_ptr<WriteThroughMemoryBuffer> WriteThroughMemoryBuffer::GetFile(
    std::string_view filename, std::error_code& ec, int64_t fileSize) {
  return GetReadWriteFile(filename, ec, fileSize, fileSize, 0);
}

/// Map a subrange of the specified file as a WritableMemoryBuffer.
std::unique_ptr<WriteThroughMemoryBuffer>
WriteThroughMemoryBuffer::GetFileSlice(std::string_view filename,
                                       std::error_code& ec, uint64_t mapSize,
                                       uint64_t offset) {
  return GetReadWriteFile(filename, ec, -1, mapSize, offset);
}

template <typename MB>
static std::unique_ptr<MB> GetOpenFileImpl(fs::file_t f,
                                           std::string_view filename,
                                           std::error_code& ec,
                                           uint64_t fileSize, uint64_t mapSize,
                                           int64_t offset) {
  // Default is to map the full file.
  if (mapSize == uint64_t(-1)) {
    // If we don't know the file size, use fstat to find out.  fstat on an open
    // file descriptor is cheaper than stat on a random path.
    if (fileSize == uint64_t(-1)) {
#ifdef _WIN32
      // If this not a file or a block device (e.g. it's a named pipe
      // or character device), we can't trust the size. Create the memory
      // buffer by copying off the stream.
      LARGE_INTEGER fileSizeWin;
      if (GetFileType(f) != FILE_TYPE_DISK || !GetFileSizeEx(f, &fileSizeWin)) {
        return GetMemoryBufferForStream(f, filename, ec);
      }
      fileSize = fileSizeWin.QuadPart;
#else
      struct stat status;
      if (fstat(f, &status) < 0) {
        ec = std::error_code(errno, std::generic_category());
        return nullptr;
      }

      // If this not a file or a block device (e.g. it's a named pipe
      // or character device), we can't trust the size. Create the memory
      // buffer by copying off the stream.
      if (!S_ISREG(status.st_mode) && !S_ISBLK(status.st_mode)) {
        return GetMemoryBufferForStream(f, filename, ec);
      }

      fileSize = status.st_size;
#endif
    }
    mapSize = fileSize;
  }

  // Don't use mmap for small files
  if (mapSize >= 4 * 4096) {
    std::unique_ptr<MB> result(new (NamedBufferAlloc(
        filename)) MemoryBufferMMapFile<MB>(f, mapSize, offset, ec));
    if (!ec) {
      return result;
    }
  }

  auto buf = WritableMemoryBuffer::GetNewUninitMemBuffer(mapSize, filename);
  if (!buf) {
    // Failed to create a buffer. The only way it can fail is if
    // new(std::nothrow) returns 0.
    ec = make_error_code(errc::not_enough_memory);
    return nullptr;
  }

  uint8_t* bufPtr = buf.get()->begin();

  size_t bytesLeft = mapSize;
  while (bytesLeft) {
#ifdef _WIN32
    LARGE_INTEGER offsetWin;
    offsetWin.QuadPart = offset;
    DWORD numRead;
    if (!SetFilePointerEx(f, offsetWin, nullptr, FILE_BEGIN) ||
        !ReadFile(f, bufPtr, bytesLeft, &numRead, nullptr)) {
      ec = mapWindowsError(GetLastError());
      return nullptr;
    }
// TODO
#else
    ssize_t numRead = sys::RetryAfterSignal(-1, ::pread, f, bufPtr, bytesLeft,
                                            mapSize - bytesLeft + offset);
    if (numRead == -1) {
      // Error while reading.
      ec = std::error_code(errno, std::generic_category());
      return nullptr;
    }
#endif
    if (numRead == 0) {
      std::memset(bufPtr, 0, bytesLeft);  // zero-initialize rest of the buffer.
      break;
    }
    bytesLeft -= numRead;
    bufPtr += numRead;
  }

  return buf;
}

std::unique_ptr<MemoryBuffer> MemoryBuffer::GetOpenFile(
    fs::file_t f, std::string_view filename, std::error_code& ec,
    uint64_t fileSize) {
  return GetOpenFileImpl<MemoryBuffer>(f, filename, ec, fileSize, fileSize, 0);
}

std::unique_ptr<MemoryBuffer> MemoryBuffer::GetOpenFileSlice(
    fs::file_t f, std::string_view filename, std::error_code& ec,
    uint64_t mapSize, int64_t offset) {
  assert(mapSize != uint64_t(-1));
  return GetOpenFileImpl<MemoryBuffer>(f, filename, ec, -1, mapSize, offset);
}

std::unique_ptr<MemoryBuffer> MemoryBuffer::GetFileAsStream(
    std::string_view filename, std::error_code& ec) {
  fs::file_t f = fs::OpenFileForRead(filename, ec, fs::OF_None);
  if (ec) {
    return nullptr;
  }
  std::unique_ptr<MemoryBuffer> ret = GetMemoryBufferForStream(f, filename, ec);
  fs::CloseFile(f);
  return ret;
}

MemoryBufferRef MemoryBuffer::GetMemBufferRef() const {
  return MemoryBufferRef(GetBuffer(), GetBufferIdentifier());
}

SmallVectorMemoryBuffer::~SmallVectorMemoryBuffer() {}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//===--- MemoryBuffer.h - Memory Buffer Interface ---------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the MemoryBuffer interface.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <stdint.h>

#include <cstddef>
#include <memory>
#include <span>
#include <string_view>
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

class MemoryBufferRef;

/// This interface provides simple read-only access to a block of memory, and
/// provides simple methods for reading files and standard input into a memory
/// buffer.
class MemoryBuffer {
  const uint8_t* m_bufferStart;  // Start of the buffer.
  const uint8_t* m_bufferEnd;    // End of the buffer.

 protected:
  MemoryBuffer() = default;

  void Init(const uint8_t* bufStart, const uint8_t* bufEnd);

 public:
  MemoryBuffer(const MemoryBuffer&) = delete;
  MemoryBuffer& operator=(const MemoryBuffer&) = delete;
  virtual ~MemoryBuffer();

  const uint8_t* begin() const { return m_bufferStart; }
  const uint8_t* end() const { return m_bufferEnd; }
  size_t size() const { return m_bufferEnd - m_bufferStart; }

  std::span<const uint8_t> GetBuffer() const { return {begin(), end()}; }

  std::span<const char> GetCharBuffer() const {
    return {reinterpret_cast<const char*>(begin()),
            reinterpret_cast<const char*>(end())};
  }

  /// Return an identifier for this buffer, typically the filename it was read
  /// from.
  virtual std::string_view GetBufferIdentifier() const {
    return "Unknown buffer";
  }

  /// Open the specified file as a MemoryBuffer, returning a new MemoryBuffer
  /// if successful, otherwise returning null. If FileSize is specified, this
  /// means that the client knows that the file exists and that it has the
  /// specified size.
  static std::unique_ptr<MemoryBuffer> GetFile(std::string_view filename,
                                               std::error_code& ec,
                                               int64_t fileSize = -1);

  /// Read all of the specified file into a MemoryBuffer as a stream
  /// (i.e. until EOF reached). This is useful for special files that
  /// look like a regular file but have 0 size (e.g. /proc/cpuinfo on Linux).
  static std::unique_ptr<MemoryBuffer> GetFileAsStream(
      std::string_view filename, std::error_code& ec);

  /// Given an already-open file descriptor, map some slice of it into a
  /// MemoryBuffer. The slice is specified by an \p Offset and \p MapSize.
  static std::unique_ptr<MemoryBuffer> GetOpenFileSlice(
      fs::file_t f, std::string_view filename, std::error_code& ec,
      uint64_t mapSize, int64_t offset);

  /// Given an already-open file descriptor, read the file and return a
  /// MemoryBuffer.
  static std::unique_ptr<MemoryBuffer> GetOpenFile(fs::file_t f,
                                                   std::string_view filename,
                                                   std::error_code& ec,
                                                   uint64_t fileSize);

  /// Open the specified memory range as a MemoryBuffer.
  static std::unique_ptr<MemoryBuffer> GetMemBuffer(
      std::span<const uint8_t> inputData, std::string_view bufferName = "");

  static std::unique_ptr<MemoryBuffer> GetMemBuffer(MemoryBufferRef ref);

  /// Open the specified memory range as a MemoryBuffer, copying the contents
  /// and taking ownership of it.
  static std::unique_ptr<MemoryBuffer> GetMemBufferCopy(
      std::span<const uint8_t> inputData, std::string_view bufferName = "");

  /// Map a subrange of the specified file as a MemoryBuffer.
  static std::unique_ptr<MemoryBuffer> GetFileSlice(std::string_view filename,
                                                    std::error_code& ec,
                                                    uint64_t mapSize,
                                                    uint64_t offset);

  //===--------------------------------------------------------------------===//
  // Provided for performance analysis.
  //===--------------------------------------------------------------------===//

  /// The kind of memory backing used to support the MemoryBuffer.
  enum BufferKind { MemoryBuffer_Malloc, MemoryBuffer_MMap };

  /// Return information on the memory mechanism used to support the
  /// MemoryBuffer.
  virtual BufferKind GetBufferKind() const = 0;

  MemoryBufferRef GetMemBufferRef() const;
};

/// This class is an extension of MemoryBuffer, which allows copy-on-write
/// access to the underlying contents.  It only supports creation methods that
/// are guaranteed to produce a writable buffer.  For example, mapping a file
/// read-only is not supported.
class WritableMemoryBuffer : public MemoryBuffer {
 protected:
  WritableMemoryBuffer() = default;

 public:
  using MemoryBuffer::begin;
  using MemoryBuffer::end;
  using MemoryBuffer::GetBuffer;
  using MemoryBuffer::size;

  // const_cast is well-defined here, because the underlying buffer is
  // guaranteed to have been initialized with a mutable buffer.
  uint8_t* begin() { return const_cast<uint8_t*>(MemoryBuffer::begin()); }
  uint8_t* end() { return const_cast<uint8_t*>(MemoryBuffer::end()); }
  std::span<uint8_t> GetBuffer() { return {begin(), end()}; }
  std::span<char> GetCharBuffer() const {
    return {reinterpret_cast<char*>(const_cast<uint8_t*>(begin())),
            reinterpret_cast<char*>(const_cast<uint8_t*>(end()))};
  }

  static std::unique_ptr<WritableMemoryBuffer> GetFile(
      std::string_view filename, std::error_code& ec, int64_t fileSize = -1);

  /// Map a subrange of the specified file as a WritableMemoryBuffer.
  static std::unique_ptr<WritableMemoryBuffer> GetFileSlice(
      std::string_view filename, std::error_code& ec, uint64_t mapSize,
      uint64_t offset);

  /// Allocate a new MemoryBuffer of the specified size that is not initialized.
  /// Note that the caller should initialize the memory allocated by this
  /// method. The memory is owned by the MemoryBuffer object.
  static std::unique_ptr<WritableMemoryBuffer> GetNewUninitMemBuffer(
      size_t size, std::string_view bufferName = "");

  /// Allocate a new zero-initialized MemoryBuffer of the specified size. Note
  /// that the caller need not initialize the memory allocated by this method.
  /// The memory is owned by the MemoryBuffer object.
  static std::unique_ptr<WritableMemoryBuffer> GetNewMemBuffer(
      size_t size, std::string_view bufferName = "");

 private:
  // Hide these base class factory function so one can't write
  //   WritableMemoryBuffer::getXXX()
  // and be surprised that they got a read-only Buffer.
  using MemoryBuffer::GetFileAsStream;
  using MemoryBuffer::GetMemBuffer;
  using MemoryBuffer::GetMemBufferCopy;
  using MemoryBuffer::GetOpenFile;
  using MemoryBuffer::GetOpenFileSlice;
};

/// This class is an extension of MemoryBuffer, which allows write access to
/// the underlying contents and committing those changes to the original source.
/// It only supports creation methods that are guaranteed to produce a writable
/// buffer.  For example, mapping a file read-only is not supported.
class WriteThroughMemoryBuffer : public MemoryBuffer {
 protected:
  WriteThroughMemoryBuffer() = default;

 public:
  using MemoryBuffer::begin;
  using MemoryBuffer::end;
  using MemoryBuffer::GetBuffer;
  using MemoryBuffer::size;

  // const_cast is well-defined here, because the underlying buffer is
  // guaranteed to have been initialized with a mutable buffer.
  uint8_t* begin() { return const_cast<uint8_t*>(MemoryBuffer::begin()); }
  uint8_t* end() { return const_cast<uint8_t*>(MemoryBuffer::end()); }
  std::span<uint8_t> GetBuffer() { return {begin(), end()}; }
  std::span<char> GetCharBuffer() const {
    return {reinterpret_cast<char*>(const_cast<uint8_t*>(begin())),
            reinterpret_cast<char*>(const_cast<uint8_t*>(end()))};
  }

  static std::unique_ptr<WriteThroughMemoryBuffer> GetFile(
      std::string_view filename, std::error_code& ec, int64_t fileSize = -1);

  /// Map a subrange of the specified file as a ReadWriteMemoryBuffer.
  static std::unique_ptr<WriteThroughMemoryBuffer> GetFileSlice(
      std::string_view filename, std::error_code& ec, uint64_t mapSize,
      uint64_t offset);

 private:
  // Hide these base class factory function so one can't write
  //   WritableMemoryBuffer::getXXX()
  // and be surprised that they got a read-only Buffer.
  using MemoryBuffer::GetFileAsStream;
  using MemoryBuffer::GetMemBuffer;
  using MemoryBuffer::GetMemBufferCopy;
  using MemoryBuffer::GetOpenFile;
  using MemoryBuffer::GetOpenFileSlice;
};

class MemoryBufferRef {
  std::span<const uint8_t> m_buffer;
  std::string_view m_id;

 public:
  MemoryBufferRef() = default;
  MemoryBufferRef(MemoryBuffer& buffer)  // NOLINT
      : m_buffer(buffer.GetBuffer()), m_id(buffer.GetBufferIdentifier()) {}
  MemoryBufferRef(std::span<const uint8_t> buffer, std::string_view id)
      : m_buffer(buffer), m_id(id) {}

  std::span<const uint8_t> GetBuffer() const { return m_buffer; }

  std::string_view GetBufferIdentifier() const { return m_id; }

  const uint8_t* begin() const { return &*m_buffer.begin(); }
  const uint8_t* end() const { return &*m_buffer.end(); }
  size_t size() const { return m_buffer.size(); }
};

}  // namespace wpi

//===--- raw_ostream.cpp - Implement the raw_ostream classes --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This implements support for bulk buffered stream output.
//
//===----------------------------------------------------------------------===//

#ifdef _WIN32
#define _CRT_NONSTDC_NO_WARNINGS
#endif

#include "wpi/raw_ostream.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/StringExtras.h"
#include "wpi/Compiler.h"
#include "wpi/ErrorHandling.h"
#include "wpi/fs.h"
#include "wpi/MathExtras.h"
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <sys/stat.h>

// <fcntl.h> may provide O_BINARY.
# include <fcntl.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/uio.h>
#endif

#if defined(__CYGWIN__)
#include <io.h>
#endif

#if defined(_MSC_VER)
#include <io.h>
#ifndef STDIN_FILENO
# define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
# define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
# define STDERR_FILENO 2
#endif
#endif

#ifdef _WIN32
#include "wpi/ConvertUTF.h"
#include "Windows/WindowsSupport.h"
#endif

using namespace wpi;

constexpr raw_ostream::Colors raw_ostream::BLACK;
constexpr raw_ostream::Colors raw_ostream::RED;
constexpr raw_ostream::Colors raw_ostream::GREEN;
constexpr raw_ostream::Colors raw_ostream::YELLOW;
constexpr raw_ostream::Colors raw_ostream::BLUE;
constexpr raw_ostream::Colors raw_ostream::MAGENTA;
constexpr raw_ostream::Colors raw_ostream::CYAN;
constexpr raw_ostream::Colors raw_ostream::WHITE;
constexpr raw_ostream::Colors raw_ostream::SAVEDCOLOR;
constexpr raw_ostream::Colors raw_ostream::RESET;

namespace {
// Find the length of an array.
template <class T, std::size_t N>
constexpr inline size_t array_lengthof(T (&)[N]) {
  return N;
}
}  // namespace

raw_ostream::~raw_ostream() {
  // raw_ostream's subclasses should take care to flush the buffer
  // in their destructors.
  assert(OutBufCur == OutBufStart &&
         "raw_ostream destructor called with non-empty buffer!");

  if (BufferMode == BufferKind::InternalBuffer)
    delete [] OutBufStart;
}

size_t raw_ostream::preferred_buffer_size() const {
#ifdef _WIN32
  // On Windows BUFSIZ is only 512 which results in more calls to write. This
  // overhead can cause significant performance degradation. Therefore use a
  // better default.
  return (16 * 1024);
#else
  // BUFSIZ is intended to be a reasonable default.
  return BUFSIZ;
#endif
}

void raw_ostream::SetBuffered() {
  // Ask the subclass to determine an appropriate buffer size.
  if (size_t Size = preferred_buffer_size())
    SetBufferSize(Size);
  else
    // It may return 0, meaning this stream should be unbuffered.
    SetUnbuffered();
}

void raw_ostream::SetBufferAndMode(char *BufferStart, size_t Size,
                                   BufferKind Mode) {
  assert(((Mode == BufferKind::Unbuffered && !BufferStart && Size == 0) ||
          (Mode != BufferKind::Unbuffered && BufferStart && Size != 0)) &&
         "stream must be unbuffered or have at least one byte");
  // Make sure the current buffer is free of content (we can't flush here; the
  // child buffer management logic will be in write_impl).
  assert(GetNumBytesInBuffer() == 0 && "Current buffer is non-empty!");

  if (BufferMode == BufferKind::InternalBuffer)
    delete [] OutBufStart;
  OutBufStart = BufferStart;
  OutBufEnd = OutBufStart+Size;
  OutBufCur = OutBufStart;
  BufferMode = Mode;

  assert(OutBufStart <= OutBufEnd && "Invalid size!");
}

raw_ostream &raw_ostream::write_escaped(std::string_view Str,
                                        bool UseHexEscapes) {
  for (unsigned char c : Str) {
    switch (c) {
    case '\\':
      *this << '\\' << '\\';
      break;
    case '\t':
      *this << '\\' << 't';
      break;
    case '\n':
      *this << '\\' << 'n';
      break;
    case '"':
      *this << '\\' << '"';
      break;
    default:
      if (isPrint(c)) {
        *this << c;
        break;
      }

      // Write out the escaped representation.
      if (UseHexEscapes) {
        *this << '\\' << 'x';
        *this << hexdigit((c >> 4) & 0xF);
        *this << hexdigit((c >> 0) & 0xF);
      } else {
        // Always use a full 3-character octal escape.
        *this << '\\';
        *this << char('0' + ((c >> 6) & 7));
        *this << char('0' + ((c >> 3) & 7));
        *this << char('0' + ((c >> 0) & 7));
      }
    }
  }

  return *this;
}

void raw_ostream::flush_nonempty() {
  assert(OutBufCur > OutBufStart && "Invalid call to flush_nonempty.");
  size_t Length = OutBufCur - OutBufStart;
  OutBufCur = OutBufStart;
  write_impl(OutBufStart, Length);
}

raw_ostream &raw_ostream::write(unsigned char C) {
  // Group exceptional cases into a single branch.
  if (LLVM_UNLIKELY(OutBufCur >= OutBufEnd)) {
    if (LLVM_UNLIKELY(!OutBufStart)) {
      if (BufferMode == BufferKind::Unbuffered) {
        write_impl(reinterpret_cast<char *>(&C), 1);
        return *this;
      }
      // Set up a buffer and start over.
      SetBuffered();
      return write(C);
    }

    flush_nonempty();
  }

  *OutBufCur++ = C;
  return *this;
}

raw_ostream &raw_ostream::write(const char *Ptr, size_t Size) {
  // Group exceptional cases into a single branch.
  if (LLVM_UNLIKELY(size_t(OutBufEnd - OutBufCur) < Size)) {
    if (LLVM_UNLIKELY(!OutBufStart)) {
      if (BufferMode == BufferKind::Unbuffered) {
        write_impl(Ptr, Size);
        return *this;
      }
      // Set up a buffer and start over.
      SetBuffered();
      return write(Ptr, Size);
    }

    size_t NumBytes = OutBufEnd - OutBufCur;

    // If the buffer is empty at this point we have a string that is larger
    // than the buffer. Directly write the chunk that is a multiple of the
    // preferred buffer size and put the remainder in the buffer.
    if (LLVM_UNLIKELY(OutBufCur == OutBufStart)) {
      assert(NumBytes != 0 && "undefined behavior");
      size_t BytesToWrite = Size - (Size % NumBytes);
      write_impl(Ptr, BytesToWrite);
      size_t BytesRemaining = Size - BytesToWrite;
      if (BytesRemaining > size_t(OutBufEnd - OutBufCur)) {
        // Too much left over to copy into our buffer.
        return write(Ptr + BytesToWrite, BytesRemaining);
      }
      copy_to_buffer(Ptr + BytesToWrite, BytesRemaining);
      return *this;
    }

    // We don't have enough space in the buffer to fit the string in. Insert as
    // much as possible, flush and start over with the remainder.
    copy_to_buffer(Ptr, NumBytes);
    flush_nonempty();
    return write(Ptr + NumBytes, Size - NumBytes);
  }

  copy_to_buffer(Ptr, Size);

  return *this;
}

void raw_ostream::copy_to_buffer(const char *Ptr, size_t Size) {
  assert(Size <= size_t(OutBufEnd - OutBufCur) && "Buffer overrun!");

  // Handle short strings specially, memcpy isn't very good at very short
  // strings.
  switch (Size) {
  case 4: OutBufCur[3] = Ptr[3]; [[fallthrough]];
  case 3: OutBufCur[2] = Ptr[2]; [[fallthrough]];
  case 2: OutBufCur[1] = Ptr[1]; [[fallthrough]];
  case 1: OutBufCur[0] = Ptr[0]; [[fallthrough]];
  case 0: break;
  default:
    memcpy(OutBufCur, Ptr, Size);
    break;
  }

  OutBufCur += Size;
}

template <char C>
static raw_ostream &write_padding(raw_ostream &OS, unsigned NumChars) {
  static const char Chars[] = {C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C,
                               C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C,
                               C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C,
                               C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C,
                               C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C};

  // Usually the indentation is small, handle it with a fastpath.
  if (NumChars < std::size(Chars))
    return OS.write(Chars, NumChars);

  while (NumChars) {
    unsigned NumToWrite = std::min(NumChars, (unsigned)std::size(Chars) - 1);
    OS.write(Chars, NumToWrite);
    NumChars -= NumToWrite;
  }
  return OS;
}

/// indent - Insert 'NumSpaces' spaces.
raw_ostream &raw_ostream::indent(unsigned NumSpaces) {
  return write_padding<' '>(*this, NumSpaces);
}

/// write_zeros - Insert 'NumZeros' nulls.
raw_ostream &raw_ostream::write_zeros(unsigned NumZeros) {
  return write_padding<'\0'>(*this, NumZeros);
}

void raw_ostream::anchor() {}

//===----------------------------------------------------------------------===//
//  raw_fd_ostream
//===----------------------------------------------------------------------===//

static int getFD(std::string_view Filename, std::error_code &EC,
                 fs::CreationDisposition Disp, fs::FileAccess Access,
                 fs::OpenFlags Flags) {
  assert((Access & fs::FA_Write) &&
         "Cannot make a raw_ostream from a read-only descriptor!");

  // Handle "-" as stdout. Note that when we do this, we consider ourself
  // the owner of stdout and may set the "binary" flag globally based on Flags.
  if (Filename == "-") {
    EC = std::error_code();
    // Change stdout's text/binary mode based on the Flags.
    if (!(Flags & fs::OF_Text)) {
#if defined(_WIN32)
      _setmode(_fileno(stdout), _O_BINARY);
#endif
    }
    return STDOUT_FILENO;
  }

  fs::file_t F;
  if (Access & fs::FA_Read)
    F = fs::OpenFileForReadWrite(fs::path{std::string_view{Filename.data(), Filename.size()}}, EC, Disp, Flags);
  else
    F = fs::OpenFileForWrite(fs::path{std::string_view{Filename.data(), Filename.size()}}, EC, Disp, Flags);
  if (EC)
    return -1;
  int FD = fs::FileToFd(F, EC, Flags);
  if (EC)
    return -1;

  return FD;
}

raw_fd_ostream::raw_fd_ostream(std::string_view Filename, std::error_code &EC)
    : raw_fd_ostream(Filename, EC, fs::CD_CreateAlways, fs::FA_Write,
                     fs::OF_None) {}

raw_fd_ostream::raw_fd_ostream(std::string_view Filename, std::error_code &EC,
                               fs::CreationDisposition Disp)
    : raw_fd_ostream(Filename, EC, Disp, fs::FA_Write, fs::OF_None) {}

raw_fd_ostream::raw_fd_ostream(std::string_view Filename, std::error_code &EC,
                               fs::FileAccess Access)
    : raw_fd_ostream(Filename, EC, fs::CD_CreateAlways, Access,
                     fs::OF_None) {}

raw_fd_ostream::raw_fd_ostream(std::string_view Filename, std::error_code &EC,
                               fs::OpenFlags Flags)
    : raw_fd_ostream(Filename, EC, fs::CD_CreateAlways, fs::FA_Write,
                     Flags) {}

raw_fd_ostream::raw_fd_ostream(std::string_view Filename, std::error_code &EC,
                               fs::CreationDisposition Disp,
                               fs::FileAccess Access,
                               fs::OpenFlags Flags)
    : raw_fd_ostream(getFD(Filename, EC, Disp, Access, Flags), true) {}

/// FD is the file descriptor that this writes to.  If ShouldClose is true, this
/// closes the file when the stream is destroyed.
raw_fd_ostream::raw_fd_ostream(int fd, bool shouldClose, bool unbuffered,
                               OStreamKind K)
    : raw_pwrite_stream(unbuffered, K), FD(fd), ShouldClose(shouldClose) {
  if (FD < 0 ) {
    ShouldClose = false;
    return;
  }

  enable_colors(true);

  // Do not attempt to close stdout or stderr. We used to try to maintain the
  // property that tools that support writing file to stdout should not also
  // write informational output to stdout, but in practice we were never able to
  // maintain this invariant. Many features have been added to LLVM and clang
  // (-fdump-record-layouts, optimization remarks, etc) that print to stdout, so
  // users must simply be aware that mixed output and remarks is a possibility.
  if (FD <= STDERR_FILENO)
    ShouldClose = false;

#ifdef _WIN32
  // Check if this is a console device. This is not equivalent to isatty.
  IsWindowsConsole =
      ::GetFileType((HANDLE)::_get_osfhandle(fd)) == FILE_TYPE_CHAR;
#endif

  // Get the starting position.
  off_t loc = ::lseek(FD, 0, SEEK_CUR);
#ifdef _WIN32
  SupportsSeeking = loc != (off_t)-1 && ::GetFileType(reinterpret_cast<HANDLE>(::_get_osfhandle(FD))) != FILE_TYPE_PIPE;
#else
  SupportsSeeking = !EC && loc != (off_t)-1;
#endif
  if (!SupportsSeeking)
    pos = 0;
  else
    pos = static_cast<uint64_t>(loc);
}

raw_fd_ostream::~raw_fd_ostream() {
  if (FD >= 0) {
    flush();
    if (ShouldClose && ::close(FD) < 0)
      error_detected(std::error_code(errno, std::generic_category()));
  }

#ifdef __MINGW32__
  // On mingw, global dtors should not call exit().
  // report_fatal_error() invokes exit(). We know report_fatal_error()
  // might not write messages to stderr when any errors were detected
  // on FD == 2.
  if (FD == 2) return;
#endif

  // If there are any pending errors, report them now. Clients wishing
  // to avoid report_fatal_error calls should check for errors with
  // has_error() and clear the error flag with clear_error() before
  // destructing raw_ostream objects which may have errors.
  if (has_error())
    report_fatal_error("IO failure on output stream: " + error().message(),
                       /*gen_crash_diag=*/false);
}

#if defined(_WIN32)
// The most reliable way to print unicode in a Windows console is with
// WriteConsoleW. To use that, first transcode from UTF-8 to UTF-16. This
// assumes that LLVM programs always print valid UTF-8 to the console. The data
// might not be UTF-8 for two major reasons:
// 1. The program is printing binary (-filetype=obj -o -), in which case it
// would have been gibberish anyway.
// 2. The program is printing text in a semi-ascii compatible codepage like
// shift-jis or cp1252.
//
// Most LLVM programs don't produce non-ascii text unless they are quoting
// user source input. A well-behaved LLVM program should either validate that
// the input is UTF-8 or transcode from the local codepage to UTF-8 before
// quoting it. If they don't, this may mess up the encoding, but this is still
// probably the best compromise we can make.
static bool write_console_impl(int FD, std::string_view Data) {
  SmallVector<wchar_t, 256> WideText;

  // Fall back to ::write if it wasn't valid UTF-8.
  if (auto EC = sys::windows::UTF8ToUTF16(Data, WideText))
    return false;

  // On Windows 7 and earlier, WriteConsoleW has a low maximum amount of data
  // that can be written to the console at a time.
  size_t MaxWriteSize = WideText.size();
  if (!RunningWindows8OrGreater())
    MaxWriteSize = 32767;

  size_t WCharsWritten = 0;
  do {
    size_t WCharsToWrite =
        std::min(MaxWriteSize, WideText.size() - WCharsWritten);
    DWORD ActuallyWritten;
    bool Success =
        ::WriteConsoleW((HANDLE)::_get_osfhandle(FD), &WideText[WCharsWritten],
                        WCharsToWrite, &ActuallyWritten,
                        /*Reserved=*/nullptr);

    // The most likely reason for WriteConsoleW to fail is that FD no longer
    // points to a console. Fall back to ::write. If this isn't the first loop
    // iteration, something is truly wrong.
    if (!Success)
      return false;

    WCharsWritten += ActuallyWritten;
  } while (WCharsWritten != WideText.size());
  return true;
}
#endif

void raw_fd_ostream::write_impl(const char *Ptr, size_t Size) {
  if (TiedStream)
    TiedStream->flush();

  assert(FD >= 0 && "File already closed.");
  pos += Size;

#if defined(_WIN32)
  // If this is a Windows console device, try re-encoding from UTF-8 to UTF-16
  // and using WriteConsoleW. If that fails, fall back to plain write().
  if (IsWindowsConsole)
    if (write_console_impl(FD, std::string_view(Ptr, Size)))
      return;
#endif

  // The maximum write size is limited to INT32_MAX. A write
  // greater than SSIZE_MAX is implementation-defined in POSIX,
  // and Windows _write requires 32 bit input.
  size_t MaxWriteSize = INT32_MAX;

#if defined(__linux__)
  // It is observed that Linux returns EINVAL for a very large write (>2G).
  // Make it a reasonably small value.
  MaxWriteSize = 1024 * 1024 * 1024;
#endif

  do {
    size_t ChunkSize = std::min(Size, MaxWriteSize);
#ifdef _WIN32
    int ret = ::_write(FD, Ptr, ChunkSize);
#else
    ssize_t ret = ::write(FD, Ptr, ChunkSize);
#endif

    if (ret < 0) {
      // If it's a recoverable error, swallow it and retry the write.
      //
      // Ideally we wouldn't ever see EAGAIN or EWOULDBLOCK here, since
      // raw_ostream isn't designed to do non-blocking I/O. However, some
      // programs, such as old versions of bjam, have mistakenly used
      // O_NONBLOCK. For compatibility, emulate blocking semantics by
      // spinning until the write succeeds. If you don't want spinning,
      // don't use O_NONBLOCK file descriptors with raw_ostream.
      if (errno == EINTR || errno == EAGAIN
#ifdef EWOULDBLOCK
          || errno == EWOULDBLOCK
#endif
          )
        continue;

#ifdef _WIN32
      // Windows equivalents of SIGPIPE/EPIPE.
      DWORD WinLastError = GetLastError();
      if (WinLastError == ERROR_BROKEN_PIPE ||
          (WinLastError == ERROR_NO_DATA && errno == EINVAL)) {
        errno = EPIPE;
      }
#endif
      // Otherwise it's a non-recoverable error. Note it and quit.
      error_detected(std::error_code(errno, std::generic_category()));
      break;
    }

    // The write may have written some or all of the data. Update the
    // size and buffer pointer to reflect the remainder that needs
    // to be written. If there are no bytes left, we're done.
    Ptr += ret;
    Size -= ret;
  } while (Size > 0);
}

void raw_fd_ostream::close() {
  assert(ShouldClose);
  ShouldClose = false;
  flush();
  if (::close(FD) < 0)
    error_detected(std::error_code(errno, std::generic_category()));
  FD = -1;
}

uint64_t raw_fd_ostream::seek(uint64_t off) {
  assert(SupportsSeeking && "Stream does not support seeking!");
  flush();
#ifdef _WIN32
  pos = ::_lseeki64(FD, off, SEEK_SET);
#else
  pos = ::lseek(FD, off, SEEK_SET);
#endif
  if (pos == (uint64_t)-1)
    error_detected(std::error_code(errno, std::generic_category()));
  return pos;
}

void raw_fd_ostream::pwrite_impl(const char *Ptr, size_t Size,
                                 uint64_t Offset) {
  uint64_t Pos = tell();
  seek(Offset);
  write(Ptr, Size);
  seek(Pos);
}

size_t raw_fd_ostream::preferred_buffer_size() const {
#if defined(_WIN32)
  // Disable buffering for console devices. Console output is re-encoded from
  // UTF-8 to UTF-16 on Windows, and buffering it would require us to split the
  // buffer on a valid UTF-8 codepoint boundary. Terminal buffering is disabled
  // below on most other OSs, so do the same thing on Windows and avoid that
  // complexity.
  if (IsWindowsConsole)
    return 0;
  return raw_ostream::preferred_buffer_size();
#else
  assert(FD >= 0 && "File not yet open!");
  struct stat statbuf;
  if (fstat(FD, &statbuf) != 0)
    return 0;

  // If this is a terminal, don't use buffering. Line buffering
  // would be a more traditional thing to do, but it's not worth
  // the complexity.
  if (S_ISCHR(statbuf.st_mode) && is_displayed())
    return 0;
  // Return the preferred block size.
  return statbuf.st_blksize;
#endif
}

void raw_fd_ostream::anchor() {}

//===----------------------------------------------------------------------===//
//  outs(), errs(), nulls()
//===----------------------------------------------------------------------===//

raw_fd_ostream &wpi::outs() {
  // Set buffer settings to model stdout behavior.
  std::error_code EC;
  static raw_fd_ostream* S = new raw_fd_ostream("-", EC, fs::OF_None);
  assert(!EC);
  return *S;
}

raw_fd_ostream &wpi::errs() {
  // Set standard error to be unbuffered and tied to outs() by default.
  static raw_fd_ostream* S = new raw_fd_ostream(STDERR_FILENO, false, true);
  return *S;
}

/// nulls() - This returns a reference to a raw_ostream which discards output.
raw_ostream &wpi::nulls() {
  static raw_null_ostream S;
  return S;
}

//===----------------------------------------------------------------------===//
// File Streams
//===----------------------------------------------------------------------===//

raw_fd_stream::raw_fd_stream(std::string_view Filename, std::error_code &EC)
    : raw_fd_ostream(getFD(Filename, EC, fs::CD_CreateAlways,
                           fs::FA_Write | fs::FA_Read,
                           fs::OF_None),
                     true, false, OStreamKind::OK_FDStream) {
  if (EC)
    return;

  if (!isRegularFile())
    EC = std::make_error_code(std::errc::invalid_argument);
}

bool raw_fd_stream::classof(const raw_ostream *OS) {
  return OS->get_kind() == OStreamKind::OK_FDStream;
}

//===----------------------------------------------------------------------===//
//  raw_string_ostream
//===----------------------------------------------------------------------===//

void raw_string_ostream::write_impl(const char *Ptr, size_t Size) {
  OS.append(Ptr, Size);
}

//===----------------------------------------------------------------------===//
//  raw_svector_ostream
//===----------------------------------------------------------------------===//

uint64_t raw_svector_ostream::current_pos() const { return OS.size(); }

void raw_svector_ostream::write_impl(const char *Ptr, size_t Size) {
  OS.append(Ptr, Ptr + Size);
}

void raw_svector_ostream::pwrite_impl(const char *Ptr, size_t Size,
                                      uint64_t Offset) {
  memcpy(OS.data() + Offset, Ptr, Size);
}

bool raw_svector_ostream::classof(const raw_ostream *OS) {
  return OS->get_kind() == OStreamKind::OK_SVecStream;
}

//===----------------------------------------------------------------------===//
//  raw_vector_ostream
//===----------------------------------------------------------------------===//

uint64_t raw_vector_ostream::current_pos() const { return OS.size(); }

void raw_vector_ostream::write_impl(const char *Ptr, size_t Size) {
  OS.insert(OS.end(), Ptr, Ptr + Size);
}

void raw_vector_ostream::pwrite_impl(const char *Ptr, size_t Size,
                                     uint64_t Offset) {
  memcpy(OS.data() + Offset, Ptr, Size);
}

//===----------------------------------------------------------------------===//
//  raw_usvector_ostream
//===----------------------------------------------------------------------===//

uint64_t raw_usvector_ostream::current_pos() const { return OS.size(); }

void raw_usvector_ostream::write_impl(const char *Ptr, size_t Size) {
  OS.append(reinterpret_cast<const uint8_t *>(Ptr),
            reinterpret_cast<const uint8_t *>(Ptr) + Size);
}

void raw_usvector_ostream::pwrite_impl(const char *Ptr, size_t Size,
                                       uint64_t Offset) {
  memcpy(OS.data() + Offset, Ptr, Size);
}

//===----------------------------------------------------------------------===//
//  raw_uvector_ostream
//===----------------------------------------------------------------------===//

uint64_t raw_uvector_ostream::current_pos() const { return OS.size(); }

void raw_uvector_ostream::write_impl(const char *Ptr, size_t Size) {
  OS.insert(OS.end(), reinterpret_cast<const uint8_t *>(Ptr),
            reinterpret_cast<const uint8_t *>(Ptr) + Size);
}

void raw_uvector_ostream::pwrite_impl(const char *Ptr, size_t Size,
                                      uint64_t Offset) {
  memcpy(OS.data() + Offset, Ptr, Size);
}

//===----------------------------------------------------------------------===//
//  raw_null_ostream
//===----------------------------------------------------------------------===//

raw_null_ostream::~raw_null_ostream() {
#ifndef NDEBUG
  // ~raw_ostream asserts that the buffer is empty. This isn't necessary
  // with raw_null_ostream, but it's better to have raw_null_ostream follow
  // the rules than to change the rules just for raw_null_ostream.
  flush();
#endif
}

void raw_null_ostream::write_impl(const char *Ptr, size_t Size) {
}

uint64_t raw_null_ostream::current_pos() const {
  return 0;
}

void raw_null_ostream::pwrite_impl(const char *Ptr, size_t Size,
                                   uint64_t Offset) {}

void raw_pwrite_stream::anchor() {}

void buffer_ostream::anchor() {}

void buffer_unique_ostream::anchor() {}

//===--- raw_ostream.cpp - Implement the raw_ostream classes --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This implements support for bulk buffered stream output.
//
//===----------------------------------------------------------------------===//

#include "llvm/raw_ostream.h"
#include "llvm/SmallString.h"
#include "llvm/SmallVector.h"
#include "llvm/StringExtras.h"
#include "llvm/Compiler.h"
#include "llvm/Format.h"
#include "llvm/MathExtras.h"
#include "llvm/WindowsError.h"
#include <cctype>
#include <cerrno>
#include <sys/stat.h>
#include <system_error>

// <fcntl.h> may provide O_BINARY.
#include <fcntl.h>

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

#if defined(_WIN32)
#include <windows.h>

/// Determines if the program is running on Windows 8 or newer. This
/// reimplements one of the helpers in the Windows 8.1 SDK, which are intended
/// to supercede raw calls to GetVersionEx. Old SDKs, Cygwin, and MinGW don't
/// yet have VersionHelpers.h, so we have our own helper.
static inline bool RunningWindows8OrGreater() {
  // Windows 8 is version 6.2, service pack 0.
  OSVERSIONINFOEXW osvi = {};
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  osvi.dwMajorVersion = 6;
  osvi.dwMinorVersion = 2;
  osvi.wServicePackMajor = 0;

  DWORDLONG Mask = 0;
  Mask = VerSetConditionMask(Mask, VER_MAJORVERSION, VER_GREATER_EQUAL);
  Mask = VerSetConditionMask(Mask, VER_MINORVERSION, VER_GREATER_EQUAL);
  Mask = VerSetConditionMask(Mask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

  return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION |
                                       VER_SERVICEPACKMAJOR,
                            Mask) != FALSE;
}

static std::error_code UTF8ToUTF16(llvm::StringRef utf8,
                                   llvm::SmallVectorImpl<wchar_t> &utf16) {
  if (!utf8.empty()) {
    int len = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.begin(),
                                    utf8.size(), utf16.begin(), 0);

    if (len == 0)
      return llvm::mapWindowsError(::GetLastError());

    utf16.reserve(len + 1);
    utf16.set_size(len);

    len = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.begin(),
                                utf8.size(), utf16.begin(), utf16.size());

    if (len == 0)
      return llvm::mapWindowsError(::GetLastError());
  }

  // Make utf16 null terminated.
  utf16.push_back(0);
  utf16.pop_back();

  return std::error_code();
}

#endif

using namespace llvm;

raw_ostream::~raw_ostream() {
  // raw_ostream's subclasses should take care to flush the buffer
  // in their destructors.
  assert(OutBufCur == OutBufStart &&
         "raw_ostream destructor called with non-empty buffer!");

  if (BufferMode == InternalBuffer)
    delete [] OutBufStart;
}

// An out of line virtual method to provide a home for the class vtable.
void raw_ostream::handle() {}

size_t raw_ostream::preferred_buffer_size() const {
  // BUFSIZ is intended to be a reasonable default.
  return BUFSIZ;
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
  assert(((Mode == Unbuffered && !BufferStart && Size == 0) ||
          (Mode != Unbuffered && BufferStart && Size != 0)) &&
         "stream must be unbuffered or have at least one byte");
  // Make sure the current buffer is free of content (we can't flush here; the
  // child buffer management logic will be in write_impl).
  assert(GetNumBytesInBuffer() == 0 && "Current buffer is non-empty!");

  if (BufferMode == InternalBuffer)
    delete [] OutBufStart;
  OutBufStart = BufferStart;
  OutBufEnd = OutBufStart+Size;
  OutBufCur = OutBufStart;
  BufferMode = Mode;

  assert(OutBufStart <= OutBufEnd && "Invalid size!");
}

raw_ostream &raw_ostream::operator<<(unsigned long N) {
  // Zero is a special case.
  if (N == 0)
    return *this << '0';

  char NumberBuffer[20];
  char *EndPtr = NumberBuffer+sizeof(NumberBuffer);
  char *CurPtr = EndPtr;

  while (N) {
    *--CurPtr = '0' + char(N % 10);
    N /= 10;
  }
  return write(CurPtr, EndPtr-CurPtr);
}

raw_ostream &raw_ostream::operator<<(long N) {
  if (N <  0) {
    *this << '-';
    // Avoid undefined behavior on LONG_MIN with a cast.
    N = -(unsigned long)N;
  }

  return this->operator<<(static_cast<unsigned long>(N));
}

raw_ostream &raw_ostream::operator<<(unsigned long long N) {
  // Output using 32-bit div/mod when possible.
  if (N == static_cast<unsigned long>(N))
    return this->operator<<(static_cast<unsigned long>(N));

  char NumberBuffer[20];
  char *EndPtr = std::end(NumberBuffer);
  char *CurPtr = EndPtr;

  while (N) {
    *--CurPtr = '0' + char(N % 10);
    N /= 10;
  }
  return write(CurPtr, EndPtr-CurPtr);
}

raw_ostream &raw_ostream::operator<<(long long N) {
  if (N < 0) {
    *this << '-';
    // Avoid undefined behavior on INT64_MIN with a cast.
    N = -(unsigned long long)N;
  }

  return this->operator<<(static_cast<unsigned long long>(N));
}

raw_ostream &raw_ostream::write_hex(unsigned long long N) {
  // Zero is a special case.
  if (N == 0)
    return *this << '0';

  char NumberBuffer[16];
  char *EndPtr = std::end(NumberBuffer);
  char *CurPtr = EndPtr;

  while (N) {
    unsigned char x = static_cast<unsigned char>(N) % 16;
    *--CurPtr = hexdigit(x, /*LowerCase*/true);
    N /= 16;
  }

  return write(CurPtr, EndPtr-CurPtr);
}

raw_ostream &raw_ostream::write_escaped(StringRef Str,
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
      if (std::isprint(c)) {
        *this << c;
        break;
      }

      // Write out the escaped representation.
      if (UseHexEscapes) {
        *this << '\\' << 'x';
        *this << hexdigit((c >> 4 & 0xF));
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

raw_ostream &raw_ostream::operator<<(const void *P) {
  *this << '0' << 'x';

  return write_hex((uintptr_t) P);
}

raw_ostream &raw_ostream::operator<<(double N) {
#ifdef _WIN32
  // On MSVCRT and compatible, output of %e is incompatible to Posix
  // by default. Number of exponent digits should be at least 2. "%+03d"
  // FIXME: Implement our formatter to here or Support/Format.h!
#if defined(__MINGW32__)
  // FIXME: It should be generic to C++11.
  if (N == 0.0 && std::signbit(N))
    return *this << "-0.000000e+00";
#else
  int fpcl = _fpclass(N);

  // negative zero
  if (fpcl == _FPCLASS_NZ)
    return *this << "-0.000000e+00";
#endif

  char buf[16];
  unsigned len;
  len = format("%e", N).snprint(buf, sizeof(buf));
  if (len <= sizeof(buf) - 2) {
    if (len >= 5 && buf[len - 5] == 'e' && buf[len - 3] == '0') {
      int cs = buf[len - 4];
      if (cs == '+' || cs == '-') {
        int c1 = buf[len - 2];
        int c0 = buf[len - 1];
        if (isdigit(static_cast<unsigned char>(c1)) &&
            isdigit(static_cast<unsigned char>(c0))) {
          // Trim leading '0': "...e+012" -> "...e+12\0"
          buf[len - 3] = c1;
          buf[len - 2] = c0;
          buf[--len] = 0;
        }
      }
    }
    return this->operator<<(buf);
  }
#endif
  return this->operator<<(format("%e", N));
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
      if (BufferMode == Unbuffered) {
        write_impl(reinterpret_cast<char*>(&C), 1);
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
      if (BufferMode == Unbuffered) {
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
  case 4: OutBufCur[3] = Ptr[3]; // FALL THROUGH
  case 3: OutBufCur[2] = Ptr[2]; // FALL THROUGH
  case 2: OutBufCur[1] = Ptr[1]; // FALL THROUGH
  case 1: OutBufCur[0] = Ptr[0]; // FALL THROUGH
  case 0: break;
  default:
    memcpy(OutBufCur, Ptr, Size);
    break;
  }

  OutBufCur += Size;
}

// Formatted output.
raw_ostream &raw_ostream::operator<<(const format_object_base &Fmt) {
  // If we have more than a few bytes left in our output buffer, try
  // formatting directly onto its end.
  size_t NextBufferSize = 127;
  size_t BufferBytesLeft = OutBufEnd - OutBufCur;
  if (BufferBytesLeft > 3) {
    size_t BytesUsed = Fmt.print(OutBufCur, BufferBytesLeft);

    // Common case is that we have plenty of space.
    if (BytesUsed <= BufferBytesLeft) {
      OutBufCur += BytesUsed;
      return *this;
    }

    // Otherwise, we overflowed and the return value tells us the size to try
    // again with.
    NextBufferSize = BytesUsed;
  }

  // If we got here, we didn't have enough space in the output buffer for the
  // string.  Try printing into a SmallVector that is resized to have enough
  // space.  Iterate until we win.
  SmallVector<char, 128> V;

  while (1) {
    V.resize(NextBufferSize);

    // Try formatting into the SmallVector.
    size_t BytesUsed = Fmt.print(V.data(), NextBufferSize);

    // If BytesUsed fit into the vector, we win.
    if (BytesUsed <= NextBufferSize)
      return write(V.data(), BytesUsed);

    // Otherwise, try again with a new size.
    assert(BytesUsed > NextBufferSize && "Didn't grow buffer!?");
    NextBufferSize = BytesUsed;
  }
}

raw_ostream &raw_ostream::operator<<(const FormattedString &FS) {
  unsigned Len = FS.Str.size(); 
  int PadAmount = FS.Width - Len;
  if (FS.RightJustify && (PadAmount > 0))
    this->indent(PadAmount);
  this->operator<<(FS.Str);
  if (!FS.RightJustify && (PadAmount > 0))
    this->indent(PadAmount);
  return *this;
}

raw_ostream &raw_ostream::operator<<(const FormattedNumber &FN) {
  if (FN.Hex) {
    unsigned Nibbles = (64 - countLeadingZeros(FN.HexValue)+3)/4;
    unsigned PrefixChars = FN.HexPrefix ? 2 : 0;
    unsigned Width = std::max(FN.Width, Nibbles + PrefixChars);

    char NumberBuffer[20] = "0x0000000000000000";
    if (!FN.HexPrefix)
      NumberBuffer[1] = '0';
    char *EndPtr = NumberBuffer+Width;
    char *CurPtr = EndPtr;
    unsigned long long N = FN.HexValue;
    while (N) {
      unsigned char x = static_cast<unsigned char>(N) % 16;
      *--CurPtr = hexdigit(x, !FN.Upper);
      N /= 16;
    }

    return write(NumberBuffer, Width);
  } else {
    // Zero is a special case.
    if (FN.DecValue == 0) {
      this->indent(FN.Width-1);
      return *this << '0';
    }
    char NumberBuffer[32];
    char *EndPtr = NumberBuffer+sizeof(NumberBuffer);
    char *CurPtr = EndPtr;
    bool Neg = (FN.DecValue < 0);
    uint64_t N = Neg ? -static_cast<uint64_t>(FN.DecValue) : FN.DecValue;
    while (N) {
      *--CurPtr = '0' + char(N % 10);
      N /= 10;
    }
    int Len = EndPtr - CurPtr;
    int Pad = FN.Width - Len;
    if (Neg) 
      --Pad;
    if (Pad > 0)
      this->indent(Pad);
    if (Neg)
      *this << '-';
    return write(CurPtr, Len);
  }
}


/// indent - Insert 'NumSpaces' spaces.
raw_ostream &raw_ostream::indent(unsigned NumSpaces) {
  static const char Spaces[] = "                                "
                               "                                "
                               "                ";

  // Usually the indentation is small, handle it with a fastpath.
  if (NumSpaces < array_lengthof(Spaces))
    return write(Spaces, NumSpaces);

  while (NumSpaces) {
    unsigned NumToWrite = std::min(NumSpaces,
                                   (unsigned)array_lengthof(Spaces)-1);
    write(Spaces, NumToWrite);
    NumSpaces -= NumToWrite;
  }
  return *this;
}


//===----------------------------------------------------------------------===//
//  Formatted Output
//===----------------------------------------------------------------------===//

// Out of line virtual method.
void format_object_base::home() {
}

//===----------------------------------------------------------------------===//
//  raw_fd_ostream
//===----------------------------------------------------------------------===//

static int getFD(StringRef Filename, std::error_code &EC,
                 sys::fs::OpenFlags Flags) {
  // Handle "-" as stdout. Note that when we do this, we consider ourself
  // the owner of stdout. This means that we can do things like close the
  // file descriptor when we're done and set the "binary" flag globally.
  if (Filename == "-") {
    EC = std::error_code();
    // If user requested binary then put stdout into binary mode if
    // possible.
    if (!(Flags & sys::fs::F_Text)) {
#if defined(_WIN32)
      _setmode(_fileno(stdout), _O_BINARY);
#endif
    }
    return STDOUT_FILENO;
  }

  int FD;

  //EC = sys::fs::openFileForWrite(Filename, FD, Flags);
  //if (EC)
  //  return -1;
#if defined(_WIN32)
  // Verify that we don't have both "append" and "excl".
  assert((!(Flags & sys::fs::F_Excl) || !(Flags & sys::fs::F_Append)) &&
         "Cannot specify both 'excl' and 'append' file creation flags!");

  SmallVector<wchar_t, 128> PathUTF16;

  EC = UTF8ToUTF16(Filename, PathUTF16);
  if (EC) return -1;

  DWORD CreationDisposition;
  if (Flags & sys::fs::F_Excl)
    CreationDisposition = CREATE_NEW;
  else if (Flags & sys::fs::F_Append)
    CreationDisposition = OPEN_ALWAYS;
  else
    CreationDisposition = CREATE_ALWAYS;

  DWORD Access = GENERIC_WRITE;
  if (Flags & sys::fs::F_RW)
    Access |= GENERIC_READ;

  HANDLE H = ::CreateFileW(PathUTF16.begin(), Access,
                           FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                           CreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

  if (H == INVALID_HANDLE_VALUE) {
    DWORD LastError = ::GetLastError();
    EC = mapWindowsError(LastError);
    return -1;
  }

  int OpenFlags = 0;
  if (Flags & sys::fs::F_Append)
    OpenFlags |= _O_APPEND;

  if (Flags & sys::fs::F_Text)
    OpenFlags |= _O_TEXT;

  FD = ::_open_osfhandle(intptr_t(H), OpenFlags);
  if (FD == -1) {
    ::CloseHandle(H);
    EC = mapWindowsError(ERROR_INVALID_HANDLE);
    return -1;
  }
#else
  // Verify that we don't have both "append" and "excl".
  assert((!(Flags & sys::fs::F_Excl) || !(Flags & sys::fs::F_Append)) &&
         "Cannot specify both 'excl' and 'append' file creation flags!");

  int OpenFlags = O_CREAT;

  if (Flags & sys::fs::F_RW)
    OpenFlags |= O_RDWR;
  else
    OpenFlags |= O_WRONLY;

  if (Flags & sys::fs::F_Append)
    OpenFlags |= O_APPEND;
  else
    OpenFlags |= O_TRUNC;

  if (Flags & sys::fs::F_Excl)
    OpenFlags |= O_EXCL;

  SmallString<128> Storage{Filename};
  while ((FD = open(Storage.c_str(), OpenFlags, 0666)) < 0) {
    if (errno != EINTR) {
      EC = std::error_code(errno, std::generic_category());
      return -1;
    }
  }
#endif

  EC = std::error_code();
  return FD;
}

raw_fd_ostream::raw_fd_ostream(StringRef Filename, std::error_code &EC,
                               sys::fs::OpenFlags Flags)
    : raw_fd_ostream(getFD(Filename, EC, Flags), true) {}

/// FD is the file descriptor that this writes to.  If ShouldClose is true, this
/// closes the file when the stream is destroyed.
raw_fd_ostream::raw_fd_ostream(int fd, bool shouldClose, bool unbuffered)
    : raw_pwrite_stream(unbuffered), FD(fd), ShouldClose(shouldClose),
      Error(false) {
  if (FD < 0 ) {
    ShouldClose = false;
    return;
  }

  // Get the starting position.
  off_t loc = ::lseek(FD, 0, SEEK_CUR);
#ifdef _WIN32
  // MSVCRT's _lseek(SEEK_CUR) doesn't return -1 for pipes.
  SupportsSeeking = loc != (off_t)-1 && ::GetFileType(reinterpret_cast<HANDLE>(::_get_osfhandle(FD))) != FILE_TYPE_PIPE;
#else
  SupportsSeeking = loc != (off_t)-1;
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
      error_detected();
  }

#ifdef __MINGW32__
  // On mingw, global dtors should not call exit().
  // report_fatal_error() invokes exit(). We know report_fatal_error()
  // might not write messages to stderr when any errors were detected
  // on FD == 2.
  if (FD == 2) return;
#endif
}


void raw_fd_ostream::write_impl(const char *Ptr, size_t Size) {
  assert(FD >= 0 && "File already closed.");
  pos += Size;

#ifndef _WIN32
  bool ShouldWriteInChunks = false;
#else
  // Writing a large size of output to Windows console returns ENOMEM. It seems
  // that, prior to Windows 8, WriteFile() is redirecting to WriteConsole(), and
  // the latter has a size limit (66000 bytes or less, depending on heap usage).
  bool ShouldWriteInChunks = !!::_isatty(FD) && !RunningWindows8OrGreater();
#endif

  do {
    size_t ChunkSize = Size;
    if (ChunkSize > 32767 && ShouldWriteInChunks)
        ChunkSize = 32767;

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

      // Otherwise it's a non-recoverable error. Note it and quit.
      error_detected();
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
    error_detected();
  FD = -1;
}

uint64_t raw_fd_ostream::seek(uint64_t off) {
  assert(SupportsSeeking && "Stream does not support seeking!");
  flush();
  pos = ::lseek(FD, off, SEEK_SET);
  if (pos == (uint64_t)-1)
    error_detected();
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
#if !defined(_MSC_VER) && !defined(__MINGW32__) && !defined(__minix)
  // Windows and Minix have no st_blksize.
  assert(FD >= 0 && "File not yet open!");
  struct stat statbuf;
  if (fstat(FD, &statbuf) != 0)
    return 0;

  // If this is a terminal, don't use buffering. Line buffering
  // would be a more traditional thing to do, but it's not worth
  // the complexity.
  if (S_ISCHR(statbuf.st_mode) && isatty(FD))
    return 0;
  // Return the preferred block size.
  return statbuf.st_blksize;
#else
  return raw_ostream::preferred_buffer_size();
#endif
}

//===----------------------------------------------------------------------===//
//  outs(), errs(), nulls()
//===----------------------------------------------------------------------===//

/// outs() - This returns a reference to a raw_ostream for standard output.
/// Use it like: outs() << "foo" << "bar";
raw_ostream &llvm::outs() {
  // Set buffer settings to model stdout behavior.  Delete the file descriptor
  // when the program exits, forcing error detection.  This means that if you
  // ever call outs(), you can't open another raw_fd_ostream on stdout, as we'll
  // close stdout twice and print an error the second time.
  std::error_code EC;
  static raw_fd_ostream S("-", EC, sys::fs::F_None);
  assert(!EC);
  return S;
}

/// errs() - This returns a reference to a raw_ostream for standard error.
/// Use it like: errs() << "foo" << "bar";
raw_ostream &llvm::errs() {
  // Set standard error to be unbuffered by default.
  static raw_fd_ostream S(STDERR_FILENO, false, true);
  return S;
}

/// nulls() - This returns a reference to a raw_ostream which discards output.
raw_ostream &llvm::nulls() {
  static raw_null_ostream S;
  return S;
}


//===----------------------------------------------------------------------===//
//  raw_string_ostream
//===----------------------------------------------------------------------===//

raw_string_ostream::~raw_string_ostream() {
  flush();
}

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

void raw_null_ostream::write_impl(const char * /*Ptr*/, size_t /*Size*/) {}

uint64_t raw_null_ostream::current_pos() const {
  return 0;
}

void raw_null_ostream::pwrite_impl(const char * /*Ptr*/, size_t /*Size*/,
                                   uint64_t /*Offset*/) {}

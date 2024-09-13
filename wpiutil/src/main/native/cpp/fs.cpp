// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <cassert>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
// Require at least Windows 7 API.
#define _WIN32_WINNT 0x0601
#define _WIN32_IE 0x0800  // MinGW at it again. FIXME: verify if still needed.
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <winternl.h>
#include <ntstatus.h>

#include <shellapi.h>
#include <shlobj.h>

#include "wpi/WindowsError.h"

#else  // _WIN32

#include <fcntl.h>
#include <unistd.h>

#endif  // _WIN32

#include "wpi/Errno.h"
#include "wpi/ErrorHandling.h"
#include "wpi/WindowsError.h"
#include "wpi/fs.h"

namespace fs {

#ifdef _WIN32

#ifdef _MSC_VER
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma warning(push)
#pragma warning(disable : 4244 4267 4146)
#endif

const file_t kInvalidFile = INVALID_HANDLE_VALUE;

static DWORD nativeDisposition(CreationDisposition Disp, OpenFlags Flags) {
  switch (Disp) {
    case CD_CreateAlways:
      return CREATE_ALWAYS;
    case CD_CreateNew:
      return CREATE_NEW;
    case CD_OpenAlways:
      return OPEN_ALWAYS;
    case CD_OpenExisting:
      return OPEN_EXISTING;
  }
  wpi_unreachable("unreachable!");
}

static DWORD nativeAccess(FileAccess Access, OpenFlags Flags) {
  DWORD Result = 0;
  if (Access & FA_Read)
    Result |= GENERIC_READ;
  if (Access & FA_Write)
    Result |= GENERIC_WRITE;
  if (Flags & OF_Delete)
    Result |= DELETE;
  if (Flags & OF_UpdateAtime)
    Result |= FILE_WRITE_ATTRIBUTES;
  return Result;
}

static file_t openFileInternal(const path& Path, std::error_code& EC,
                               DWORD Disp, DWORD Access, DWORD Flags,
                               bool Inherit = false) {
  SECURITY_ATTRIBUTES SA;
  SA.nLength = sizeof(SA);
  SA.lpSecurityDescriptor = nullptr;
  SA.bInheritHandle = Inherit;

  HANDLE H =
      ::CreateFileW(Path.c_str(), Access,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, &SA,
                    Disp, Flags, NULL);
  if (H == INVALID_HANDLE_VALUE) {
    DWORD LastError = ::GetLastError();
    EC = wpi::mapWindowsError(LastError);
    // Provide a better error message when trying to open directories.
    // This only runs if we failed to open the file, so there is probably
    // no performances issues.
    if (LastError != ERROR_ACCESS_DENIED) {
      return kInvalidFile;
    }
    if (is_directory(Path)) {
      EC = std::make_error_code(std::errc::is_a_directory);
    }
    return kInvalidFile;
  }
  EC = std::error_code();
  return H;
}

static std::error_code setDeleteDisposition(HANDLE Handle, bool Delete) {
  FILE_DISPOSITION_INFO Disposition;
  Disposition.DeleteFile = Delete;
  if (!::SetFileInformationByHandle(Handle, FileDispositionInfo, &Disposition,
                                    sizeof(Disposition)))
    return wpi::mapWindowsError(::GetLastError());
  return std::error_code();
}

file_t OpenFile(const path& Path, std::error_code& EC, CreationDisposition Disp,
                FileAccess Access, OpenFlags Flags, unsigned Mode) {
  // Verify that we don't have both "append" and "excl".
  assert((!(Disp == CD_CreateNew) || !(Flags & OF_Append)) &&
         "Cannot specify both 'CreateNew' and 'Append' file creation flags!");

  DWORD NativeDisp = nativeDisposition(Disp, Flags);
  DWORD NativeAccess = nativeAccess(Access, Flags);

  bool Inherit = false;
  if (Flags & OF_ChildInherit) {
    Inherit = true;
  }

  file_t Result = openFileInternal(Path, EC, NativeDisp, NativeAccess,
                                   FILE_ATTRIBUTE_NORMAL, Inherit);
  if (EC) {
    return Result;
  }

  if (Flags & OF_UpdateAtime) {
    FILETIME FileTime;
    SYSTEMTIME SystemTime;
    ::GetSystemTime(&SystemTime);
    if (::SystemTimeToFileTime(&SystemTime, &FileTime) == 0 ||
        ::SetFileTime(Result, NULL, &FileTime, NULL) == 0) {
      DWORD LastError = ::GetLastError();
      ::CloseHandle(Result);
      EC = wpi::mapWindowsError(LastError);
      return kInvalidFile;
    }
  }

  if (Flags & OF_Delete) {
    if ((EC = setDeleteDisposition(Result, true))) {
      ::CloseHandle(Result);
      return kInvalidFile;
    }
  }
  return Result;
}

file_t OpenFileForRead(const path& Path, std::error_code& EC, OpenFlags Flags) {
  return OpenFile(Path, EC, CD_OpenExisting, FA_Read, Flags);
}

int FileToFd(file_t& F, std::error_code& EC, OpenFlags Flags) {
  if (F == kInvalidFile) {
    EC = wpi::mapWindowsError(ERROR_INVALID_HANDLE);
    return -1;
  }

  int CrtOpenFlags = 0;
  if (Flags & OF_Append) {
    CrtOpenFlags |= _O_APPEND;
  }

  if (Flags & OF_Text) {
    CrtOpenFlags |= _O_TEXT;
  }

  int ResultFD = ::_open_osfhandle(intptr_t(F), CrtOpenFlags);
  if (ResultFD == -1) {
    ::CloseHandle(F);
    EC = wpi::mapWindowsError(ERROR_INVALID_HANDLE);
    return -1;
  }

  EC = std::error_code();
  F = kInvalidFile;
  return ResultFD;
}

void CloseFile(file_t& F) {
  ::CloseHandle(F);
  F = kInvalidFile;
}

#else  // _WIN32

const file_t kInvalidFile = -1;

static int nativeOpenFlags(CreationDisposition Disp, OpenFlags Flags,
                           FileAccess Access) {
  int Result = 0;
  if (Access == FA_Read) {
    Result |= O_RDONLY;
  } else if (Access == FA_Write) {
    Result |= O_WRONLY;
  } else if (Access == (FA_Read | FA_Write)) {
    Result |= O_RDWR;
  }

  if (Disp == CD_CreateNew) {
    Result |= O_CREAT;  // Create if it doesn't exist.
    Result |= O_EXCL;   // Fail if it does.
  } else if (Disp == CD_CreateAlways) {
    Result |= O_CREAT;  // Create if it doesn't exist.
    Result |= O_TRUNC;  // Truncate if it does.
  } else if (Disp == CD_OpenAlways) {
    Result |= O_CREAT;  // Create if it doesn't exist.
  } else if (Disp == CD_OpenExisting) {
    // Nothing special, just don't add O_CREAT and we get these semantics.
  }

  if (Flags & F_Append) {
    Result |= O_APPEND;
  }

#ifdef O_CLOEXEC
  if (!(Flags & OF_ChildInherit)) {
    Result |= O_CLOEXEC;
  }
#endif

  return Result;
}

file_t OpenFile(const path& Path, std::error_code& EC, CreationDisposition Disp,
                FileAccess Access, OpenFlags Flags, unsigned Mode) {
  int OpenFlags = nativeOpenFlags(Disp, Flags, Access);
  file_t ResultFD = kInvalidFile;

  // Call ::open in a lambda to avoid overload resolution in RetryAfterSignal
  // when open is overloaded, such as in Bionic.
  auto Open = [&]() { return ::open(Path.c_str(), OpenFlags, Mode); };
  if ((ResultFD = wpi::sys::RetryAfterSignal(-1, Open)) < 0) {
    EC = std::error_code(errno, std::generic_category());
    return kInvalidFile;
  }
#ifndef O_CLOEXEC
  if (!(Flags & OF_ChildInherit)) {
    int r = fcntl(ResultFD, F_SETFD, FD_CLOEXEC);
    (void)r;
    assert(r == 0 && "fcntl(F_SETFD, FD_CLOEXEC) failed");
  }
#endif
  EC = std::error_code();
  return ResultFD;
}

file_t OpenFileForRead(const path& Path, std::error_code& EC, OpenFlags Flags) {
  return OpenFile(Path, EC, CD_OpenExisting, FA_Read, Flags, 0666);
}

int FileToFd(file_t& F, std::error_code& EC, OpenFlags Flags) {
  int fd = F;
  F = kInvalidFile;
  EC = std::error_code();
  return fd;
}

void CloseFile(file_t& F) {
  ::close(F);
  F = kInvalidFile;
}

#endif  // _WIN32

}  // namespace fs

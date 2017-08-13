//===- llvm/Support/Windows/Path.inc - Windows Path Impl --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Windows specific implementation of the Path API.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only generic Windows code that
//===          is guaranteed to work on *all* Windows variants.
//===----------------------------------------------------------------------===//

#include "llvm/STLExtras.h"
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <windows.h>
#include <shlobj.h>

#include "llvm/WindowsError.h"

#ifdef _MSC_VER
# pragma comment(lib, "shell32.lib")
# pragma comment(lib, "ole32.lib")
#endif

namespace llvm {
namespace sys  {
namespace windows {
std::error_code UTF8ToUTF16(llvm::StringRef utf8,
                            llvm::SmallVectorImpl<wchar_t> &utf16) {
  if (!utf8.empty()) {
    int len = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.begin(),
                                    utf8.size(), utf16.begin(), 0);

    if (len == 0)
      return mapWindowsError(::GetLastError());

    utf16.reserve(len + 1);
    utf16.set_size(len);

    len = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.begin(),
                                utf8.size(), utf16.begin(), utf16.size());

    if (len == 0)
      return mapWindowsError(::GetLastError());
  }

  // Make utf16 null terminated.
  utf16.push_back(0);
  utf16.pop_back();

  return std::error_code();
}

static
std::error_code UTF16ToCodePage(unsigned codepage, const wchar_t *utf16,
                                size_t utf16_len,
                                llvm::SmallVectorImpl<char> &utf8) {
  if (utf16_len) {
    // Get length.
    int len = ::WideCharToMultiByte(codepage, 0, utf16, utf16_len, utf8.begin(),
                                    0, NULL, NULL);

    if (len == 0)
      return mapWindowsError(::GetLastError());

    utf8.reserve(len);
    utf8.set_size(len);

    // Now do the actual conversion.
    len = ::WideCharToMultiByte(codepage, 0, utf16, utf16_len, utf8.data(),
                                utf8.size(), NULL, NULL);

    if (len == 0)
      return mapWindowsError(::GetLastError());
  }

  // Make utf8 null terminated.
  utf8.push_back(0);
  utf8.pop_back();

  return std::error_code();
}

std::error_code UTF16ToUTF8(const wchar_t *utf16, size_t utf16_len,
                            llvm::SmallVectorImpl<char> &utf8) {
  return UTF16ToCodePage(CP_UTF8, utf16, utf16_len, utf8);
}

std::error_code UTF16ToCurCP(const wchar_t *utf16, size_t utf16_len,
                             llvm::SmallVectorImpl<char> &utf8) {
  return UTF16ToCodePage(CP_ACP, utf16, utf16_len, utf8);
}

} // end namespace windows

using llvm::sys::windows::UTF8ToUTF16;
using llvm::sys::windows::UTF16ToUTF8;

namespace fs {

std::error_code current_path(SmallVectorImpl<char> &result) {
  SmallVector<wchar_t, MAX_PATH> cur_path;
  DWORD len = MAX_PATH;

  do {
    cur_path.reserve(len);
    len = ::GetCurrentDirectoryW(cur_path.capacity(), cur_path.data());

    // A zero return value indicates a failure other than insufficient space.
    if (len == 0)
      return mapWindowsError(::GetLastError());

    // If there's insufficient space, the len returned is larger than the len
    // given.
  } while (len > cur_path.capacity());

  // On success, GetCurrentDirectoryW returns the number of characters not
  // including the null-terminator.
  cur_path.set_size(len);
  return UTF16ToUTF8(cur_path.begin(), cur_path.size(), result);
}

} // end namespace fs

namespace path {

// Convert a UTF-8 path to UTF-16.  Also, if the absolute equivalent of the
// path is longer than CreateDirectory can tolerate, make it absolute and
// prefixed by '\\?\'.
std::error_code widenPath(const Twine &Path8,
                          SmallVectorImpl<wchar_t> &Path16) {
  const size_t MaxDirLen = MAX_PATH - 12; // Must leave room for 8.3 filename.

  // Several operations would convert Path8 to SmallString; more efficient to
  // do it once up front.
  SmallString<128> Path8Str;
  Path8.toVector(Path8Str);

  // If we made this path absolute, how much longer would it get?
  size_t CurPathLen;
  if (llvm::sys::path::is_absolute(Twine(Path8Str)))
    CurPathLen = 0; // No contribution from current_path needed.
  else {
    CurPathLen = ::GetCurrentDirectoryW(0, NULL);
    if (CurPathLen == 0)
      return mapWindowsError(::GetLastError());
  }

  // Would the absolute path be longer than our limit?
  if ((Path8Str.size() + CurPathLen) >= MaxDirLen &&
      !Path8Str.startswith("\\\\?\\")) {
    SmallString<2*MAX_PATH> FullPath("\\\\?\\");
    if (CurPathLen) {
      SmallString<80> CurPath;
      if (std::error_code EC = llvm::sys::fs::current_path(CurPath))
        return EC;
      FullPath.append(CurPath);
    }
    // Traverse the requested path, canonicalizing . and .. as we go (because
    // the \\?\ prefix is documented to treat them as real components).
    // The iterators don't report separators and append() always attaches
    // preferred_separator so we don't need to call native() on the result.
    for (llvm::sys::path::const_iterator I = llvm::sys::path::begin(Path8Str),
                                         E = llvm::sys::path::end(Path8Str);
                                         I != E; ++I) {
      if (I->size() == 1 && *I == ".")
        continue;
      if (I->size() == 2 && *I == "..")
        llvm::sys::path::remove_filename(FullPath);
      else
        llvm::sys::path::append(FullPath, *I);
    }
    return UTF8ToUTF16(FullPath, Path16);
  }

  // Just use the caller's original path.
  return UTF8ToUTF16(Path8Str, Path16);
}
} // end namespace path

using llvm::sys::path::widenPath;

namespace fs {

std::error_code openFileForRead(const Twine &Name, int &ResultFD,
                                SmallVectorImpl<char> *RealPath) {
  SmallVector<wchar_t, 128> PathUTF16;

  if (std::error_code EC = widenPath(Name, PathUTF16))
    return EC;

  HANDLE H =
      ::CreateFileW(PathUTF16.begin(), GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (H == INVALID_HANDLE_VALUE) {
    DWORD LastError = ::GetLastError();
    std::error_code EC = mapWindowsError(LastError);
    // Provide a better error message when trying to open directories.
    // This only runs if we failed to open the file, so there is probably
    // no performances issues.
    //if (LastError != ERROR_ACCESS_DENIED)
    //  return EC;
    //if (is_directory(Name))
    //  return make_error_code(errc::is_a_directory);
    return EC;
  }

  int FD = ::_open_osfhandle(intptr_t(H), 0);
  if (FD == -1) {
    ::CloseHandle(H);
    return mapWindowsError(ERROR_INVALID_HANDLE);
  }

  // Fetch the real name of the file, if the user asked
  if (RealPath) {
    RealPath->clear();
    wchar_t RealPathUTF16[MAX_PATH];
    DWORD CountChars =
      ::GetFinalPathNameByHandleW(H, RealPathUTF16, MAX_PATH,
                                  FILE_NAME_NORMALIZED);
    if (CountChars > 0 && CountChars < MAX_PATH) {
      // Convert the result from UTF-16 to UTF-8.
      SmallString<MAX_PATH> RealPathUTF8;
      if (!UTF16ToUTF8(RealPathUTF16, CountChars, RealPathUTF8))
        RealPath->append(RealPathUTF8.data(),
                         RealPathUTF8.data() + strlen(RealPathUTF8.data()));
    }
  }

  ResultFD = FD;
  return std::error_code();
}

std::error_code openFileForWrite(const Twine &Name, int &ResultFD,
                                 OpenFlags Flags, unsigned Mode) {
  // Verify that we don't have both "append" and "excl".
  assert((!(Flags & F_Excl) || !(Flags & F_Append)) &&
         "Cannot specify both 'excl' and 'append' file creation flags!");

  SmallVector<wchar_t, 128> PathUTF16;

  if (std::error_code EC = widenPath(Name, PathUTF16))
    return EC;

  DWORD CreationDisposition;
  if (Flags & F_Excl)
    CreationDisposition = CREATE_NEW;
  else if (Flags & F_Append)
    CreationDisposition = OPEN_ALWAYS;
  else
    CreationDisposition = CREATE_ALWAYS;

  DWORD Access = GENERIC_WRITE;
  if (Flags & F_RW)
    Access |= GENERIC_READ;

  HANDLE H = ::CreateFileW(PathUTF16.begin(), Access,
                           FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                           CreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

  if (H == INVALID_HANDLE_VALUE) {
    DWORD LastError = ::GetLastError();
    std::error_code EC = mapWindowsError(LastError);
    // Provide a better error message when trying to open directories.
    // This only runs if we failed to open the file, so there is probably
    // no performances issues.
    //if (LastError != ERROR_ACCESS_DENIED)
    //  return EC;
    //if (is_directory(Name))
    //  return make_error_code(errc::is_a_directory);
    return EC;
  }

  int OpenFlags = 0;
  if (Flags & F_Append)
    OpenFlags |= _O_APPEND;

  if (Flags & F_Text)
    OpenFlags |= _O_TEXT;

  int FD = ::_open_osfhandle(intptr_t(H), OpenFlags);
  if (FD == -1) {
    ::CloseHandle(H);
    return mapWindowsError(ERROR_INVALID_HANDLE);
  }

  ResultFD = FD;
  return std::error_code();
}

} // end namespace fs

namespace path {

static bool getKnownFolderPath(KNOWNFOLDERID folderId,
                               SmallVectorImpl<char> &result) {
  wchar_t *path = nullptr;
  if (::SHGetKnownFolderPath(folderId, KF_FLAG_CREATE, nullptr, &path) != S_OK)
    return false;

  bool ok = !UTF16ToUTF8(path, ::wcslen(path), result);
  ::CoTaskMemFree(path);
  return ok;
}

bool getUserCacheDir(SmallVectorImpl<char> &Result) {
  return getKnownFolderPath(FOLDERID_LocalAppData, Result);
}

bool home_directory(SmallVectorImpl<char> &result) {
  return getKnownFolderPath(FOLDERID_Profile, result);
}

static bool getTempDirEnvVar(const wchar_t *Var, SmallVectorImpl<char> &Res) {
  SmallVector<wchar_t, 1024> Buf;
  size_t Size = 1024;
  do {
    Buf.reserve(Size);
    Size = GetEnvironmentVariableW(Var, Buf.data(), Buf.capacity());
    if (Size == 0)
      return false;

    // Try again with larger buffer.
  } while (Size > Buf.capacity());
  Buf.set_size(Size);

  return !windows::UTF16ToUTF8(Buf.data(), Size, Res);
}

static bool getTempDirEnvVar(SmallVectorImpl<char> &Res) {
  const wchar_t *EnvironmentVariables[] = {L"TMP", L"TEMP", L"USERPROFILE"};
  for (auto *Env : EnvironmentVariables) {
    if (getTempDirEnvVar(Env, Res))
      return true;
  }
  return false;
}

void system_temp_directory(bool ErasedOnReboot, SmallVectorImpl<char> &Result) {
  (void)ErasedOnReboot;
  Result.clear();

  // Check whether the temporary directory is specified by an environment var.
  // This matches GetTempPath logic to some degree. GetTempPath is not used
  // directly as it cannot handle evn var longer than 130 chars on Windows 7
  // (fixed on Windows 8).
  if (getTempDirEnvVar(Result)) {
    assert(!Result.empty() && "Unexpected empty path");
    native(Result); // Some Unix-like shells use Unix path separator in $TMP.
    //fs::make_absolute(Result); // Make it absolute if not already.
    return;
  }

  // Fall back to a system default.
  const char *DefaultResult = "C:\\Temp";
  Result.append(DefaultResult, DefaultResult + strlen(DefaultResult));
}
} // end namespace path
} // end namespace sys
} // end namespace llvm

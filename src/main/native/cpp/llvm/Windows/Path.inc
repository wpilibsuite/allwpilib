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

//===- llvm/Support/Unix/Path.inc - Unix Path Implementation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Unix specific implementation of the Path API.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only generic UNIX code that
//===          is guaranteed to work on *all* UNIX variants.
//===----------------------------------------------------------------------===//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

namespace llvm {
namespace sys  {
namespace path {

bool home_directory(SmallVectorImpl<char> &result) {
  if (char *RequestedDir = std::getenv("HOME")) {
    result.clear();
    result.append(RequestedDir, RequestedDir + strlen(RequestedDir));
    return true;
  }

  return false;
}

static bool getDarwinConfDir(bool TempDir, SmallVectorImpl<char> &Result) {
  #if defined(_CS_DARWIN_USER_TEMP_DIR) && defined(_CS_DARWIN_USER_CACHE_DIR)
  // On Darwin, use DARWIN_USER_TEMP_DIR or DARWIN_USER_CACHE_DIR.
  // macros defined in <unistd.h> on darwin >= 9
  int ConfName = TempDir ? _CS_DARWIN_USER_TEMP_DIR
                         : _CS_DARWIN_USER_CACHE_DIR;
  size_t ConfLen = confstr(ConfName, nullptr, 0);
  if (ConfLen > 0) {
    do {
      Result.resize(ConfLen);
      ConfLen = confstr(ConfName, Result.data(), Result.size());
    } while (ConfLen > 0 && ConfLen != Result.size());

    if (ConfLen > 0) {
      assert(Result.back() == 0);
      Result.pop_back();
      return true;
    }

    Result.clear();
  }
  #endif
  return false;
}

static bool getUserCacheDir(SmallVectorImpl<char> &Result) {
  // First try using XDG_CACHE_HOME env variable,
  // as specified in XDG Base Directory Specification at
  // http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
  if (const char *XdgCacheDir = std::getenv("XDG_CACHE_HOME")) {
    Result.clear();
    Result.append(XdgCacheDir, XdgCacheDir + strlen(XdgCacheDir));
    return true;
  }

  // Try Darwin configuration query
  if (getDarwinConfDir(false, Result))
    return true;

  // Use "$HOME/.cache" if $HOME is available
  if (home_directory(Result)) {
    append(Result, ".cache");
    return true;
  }

  return false;
}

static const char *getEnvTempDir() {
  // Check whether the temporary directory is specified by an environment
  // variable.
  const char *EnvironmentVariables[] = {"TMPDIR", "TMP", "TEMP", "TEMPDIR"};
  for (const char *Env : EnvironmentVariables) {
    if (const char *Dir = std::getenv(Env))
      return Dir;
  }

  return nullptr;
}

static const char *getDefaultTempDir(bool ErasedOnReboot) {
#ifdef P_tmpdir
  if ((bool)P_tmpdir)
    return P_tmpdir;
#endif

  if (ErasedOnReboot)
    return "/tmp";
  return "/var/tmp";
}

void system_temp_directory(bool ErasedOnReboot, SmallVectorImpl<char> &Result) {
  Result.clear();

  if (ErasedOnReboot) {
    // There is no env variable for the cache directory.
    if (const char *RequestedDir = getEnvTempDir()) {
      Result.append(RequestedDir, RequestedDir + strlen(RequestedDir));
      return;
    }
  }

  if (getDarwinConfDir(ErasedOnReboot, Result))
    return;

  const char *RequestedDir = getDefaultTempDir(ErasedOnReboot);
  Result.append(RequestedDir, RequestedDir + strlen(RequestedDir));
}

} // end namespace path
} // end namespace sys
} // end namespace llvm

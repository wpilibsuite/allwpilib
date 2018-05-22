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

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

namespace wpi {
namespace sys  {
namespace fs {
UniqueID file_status::getUniqueID() const {
  return UniqueID(fs_st_dev, fs_st_ino);
}

std::error_code current_path(SmallVectorImpl<char> &result) {
  result.clear();

  const char *pwd = ::getenv("PWD");
  wpi::sys::fs::file_status PWDStatus, DotStatus;
  if (pwd && wpi::sys::path::is_absolute(pwd) &&
      !wpi::sys::fs::status(pwd, PWDStatus) &&
      !wpi::sys::fs::status(".", DotStatus) &&
      PWDStatus.getUniqueID() == DotStatus.getUniqueID()) {
    result.append(pwd, pwd + strlen(pwd));
    return std::error_code();
  }

#ifdef MAXPATHLEN
  result.reserve(MAXPATHLEN);
#else
  result.reserve(1024);
#endif

  while (true) {
    if (::getcwd(result.data(), result.capacity()) == nullptr) {
      // See if there was a real error.
      if (errno != ENOMEM)
        return std::error_code(errno, std::generic_category());
      // Otherwise there just wasn't enough space.
      result.reserve(result.capacity() * 2);
    } else
      break;
  }

  result.set_size(strlen(result.data()));
  return std::error_code();
}

static int convertAccessMode(AccessMode Mode) {
  switch (Mode) {
  case AccessMode::Exist:
    return F_OK;
  case AccessMode::Write:
    return W_OK;
  case AccessMode::Execute:
    return R_OK | X_OK; // scripts also need R_OK.
  default:
    return F_OK;
  }
}

std::error_code access(const Twine &Path, AccessMode Mode) {
  SmallString<128> PathStorage;
  StringRef P = Path.toNullTerminatedStringRef(PathStorage);

  if (::access(P.begin(), convertAccessMode(Mode)) == -1)
    return std::error_code(errno, std::generic_category());

  if (Mode == AccessMode::Execute) {
    // Don't say that directories are executable.
    struct stat buf;
    if (0 != stat(P.begin(), &buf))
      return std::make_error_code(std::errc::permission_denied);
    if (!S_ISREG(buf.st_mode))
      return std::make_error_code(std::errc::permission_denied);
  }

  return std::error_code();
}

bool equivalent(file_status A, file_status B) {
  assert(status_known(A) && status_known(B));
  return A.fs_st_dev == B.fs_st_dev &&
         A.fs_st_ino == B.fs_st_ino;
}

std::error_code equivalent(const Twine &A, const Twine &B, bool &result) {
  file_status fsA, fsB;
  if (std::error_code ec = status(A, fsA))
    return ec;
  if (std::error_code ec = status(B, fsB))
    return ec;
  result = equivalent(fsA, fsB);
  return std::error_code();
}

static std::error_code fillStatus(int StatRet, const struct stat &Status,
                             file_status &Result) {
  if (StatRet != 0) {
    std::error_code ec(errno, std::generic_category());
    if (ec == std::errc::no_such_file_or_directory)
      Result = file_status(file_type::file_not_found);
    else
      Result = file_status(file_type::status_error);
    return ec;
  }

  file_type Type = file_type::type_unknown;

  if (S_ISDIR(Status.st_mode))
    Type = file_type::directory_file;
  else if (S_ISREG(Status.st_mode))
    Type = file_type::regular_file;
  else if (S_ISBLK(Status.st_mode))
    Type = file_type::block_file;
  else if (S_ISCHR(Status.st_mode))
    Type = file_type::character_file;
  else if (S_ISFIFO(Status.st_mode))
    Type = file_type::fifo_file;
  else if (S_ISSOCK(Status.st_mode))
    Type = file_type::socket_file;
  else if (S_ISLNK(Status.st_mode))
    Type = file_type::symlink_file;

  perms Perms = static_cast<perms>(Status.st_mode) & all_perms;
  Result = file_status(Type, Perms, Status.st_dev, Status.st_nlink,
                       Status.st_ino, Status.st_atime, Status.st_mtime,
                       Status.st_uid, Status.st_gid, Status.st_size);

  return std::error_code();
}

std::error_code status(const Twine &Path, file_status &Result, bool Follow) {
  SmallString<128> PathStorage;
  StringRef P = Path.toNullTerminatedStringRef(PathStorage);

  struct stat Status;
  int StatRet = (Follow ? ::stat : ::lstat)(P.begin(), &Status);
  return fillStatus(StatRet, Status, Result);
}

std::error_code status(int FD, file_status &Result) {
  struct stat Status;
  int StatRet = ::fstat(FD, &Status);
  return fillStatus(StatRet, Status, Result);
}

std::error_code detail::directory_iterator_construct(detail::DirIterState &it,
                                                     StringRef path,
                                                     bool follow_symlinks) {
  SmallString<128> path_null(path);
  DIR *directory = ::opendir(path_null.c_str());
  if (!directory)
    return std::error_code(errno, std::generic_category());

  it.IterationHandle = reinterpret_cast<intptr_t>(directory);
  // Add something for replace_filename to replace.
  path::append(path_null, ".");
  it.CurrentEntry = directory_entry(path_null.str(), follow_symlinks);
  return directory_iterator_increment(it);
}

std::error_code detail::directory_iterator_destruct(detail::DirIterState &it) {
  if (it.IterationHandle)
    ::closedir(reinterpret_cast<DIR *>(it.IterationHandle));
  it.IterationHandle = 0;
  it.CurrentEntry = directory_entry();
  return std::error_code();
}

std::error_code detail::directory_iterator_increment(detail::DirIterState &it) {
  errno = 0;
  dirent *cur_dir = ::readdir(reinterpret_cast<DIR *>(it.IterationHandle));
  if (cur_dir == nullptr && errno != 0) {
    return std::error_code(errno, std::generic_category());
  } else if (cur_dir != nullptr) {
    StringRef name(cur_dir->d_name, NAMLEN(cur_dir));
    if ((name.size() == 1 && name[0] == '.') ||
        (name.size() == 2 && name[0] == '.' && name[1] == '.'))
      return directory_iterator_increment(it);
    it.CurrentEntry.replace_filename(name);
  } else
    return directory_iterator_destruct(it);

  return std::error_code();
}

ErrorOr<basic_file_status> directory_entry::status() const {
  file_status s;
  if (auto EC = fs::status(Path, s, FollowSymlinks))
    return EC;
  return s;
}

#if !defined(F_GETPATH)
static bool hasProcSelfFD() {
  // If we have a /proc filesystem mounted, we can quickly establish the
  // real name of the file with readlink
  static const bool Result = (::access("/proc/self/fd", R_OK) == 0);
  return Result;
}
#endif

std::error_code openFileForRead(const Twine &Name, int &ResultFD,
                                SmallVectorImpl<char> *RealPath) {
  SmallString<128> Storage;
  StringRef P = Name.toNullTerminatedStringRef(Storage);
  while ((ResultFD = open(P.begin(), O_RDONLY)) < 0) {
    if (errno != EINTR)
      return std::error_code(errno, std::generic_category());
  }
  // Attempt to get the real name of the file, if the user asked
  if(!RealPath)
    return std::error_code();
  RealPath->clear();
#if defined(F_GETPATH)
  // When F_GETPATH is availble, it is the quickest way to get
  // the real path name.
  char Buffer[MAXPATHLEN];
  if (::fcntl(ResultFD, F_GETPATH, Buffer) != -1)
    RealPath->append(Buffer, Buffer + strlen(Buffer));
#else
  char Buffer[PATH_MAX];
  if (hasProcSelfFD()) {
    char ProcPath[64];
    snprintf(ProcPath, sizeof(ProcPath), "/proc/self/fd/%d", ResultFD);
    ssize_t CharCount = ::readlink(ProcPath, Buffer, sizeof(Buffer));
    if (CharCount > 0)
      RealPath->append(Buffer, Buffer + CharCount);
  } else {
    // Use ::realpath to get the real path name
    if (::realpath(P.begin(), Buffer) != nullptr)
      RealPath->append(Buffer, Buffer + strlen(Buffer));
  }
#endif
  return std::error_code();
}

std::error_code openFileForWrite(const Twine &Name, int &ResultFD,
                                 OpenFlags Flags, unsigned Mode) {
  // Verify that we don't have both "append" and "excl".
  assert((!(Flags & F_Excl) || !(Flags & F_Append)) &&
         "Cannot specify both 'excl' and 'append' file creation flags!");

  int OpenFlags = O_CREAT;

#ifdef O_CLOEXEC
  OpenFlags |= O_CLOEXEC;
#endif

  if (Flags & F_RW)
    OpenFlags |= O_RDWR;
  else
    OpenFlags |= O_WRONLY;

  if (Flags & F_Append)
    OpenFlags |= O_APPEND;
  else if (!(Flags & F_NoTrunc))
    OpenFlags |= O_TRUNC;

  if (Flags & F_Excl)
    OpenFlags |= O_EXCL;

  SmallString<128> Storage;
  StringRef P = Name.toNullTerminatedStringRef(Storage);
  while ((ResultFD = open(P.begin(), OpenFlags, Mode)) < 0) {
    if (errno != EINTR)
      return std::error_code(errno, std::generic_category());
  }
  return std::error_code();
}

} // end namespace fs

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
} // end namespace wpi

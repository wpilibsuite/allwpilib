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

#include "wpi/Errno.h"
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

using namespace wpi;

namespace wpi {
namespace sys  {
namespace fs {

const file_t kInvalidFile = -1;

TimePoint<> basic_file_status::getLastAccessedTime() const {
  return toTimePoint(fs_st_atime, fs_st_atime_nsec);
}

TimePoint<> basic_file_status::getLastModificationTime() const {
  return toTimePoint(fs_st_mtime, fs_st_mtime_nsec);
}

UniqueID file_status::getUniqueID() const {
  return UniqueID(fs_st_dev, fs_st_ino);
}

uint32_t file_status::getLinkCount() const {
  return fs_st_nlinks;
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
      return errc::permission_denied;
    if (!S_ISREG(buf.st_mode))
      return errc::permission_denied;
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

static file_type typeForMode(mode_t Mode) {
  if (S_ISDIR(Mode))
    return file_type::directory_file;
  else if (S_ISREG(Mode))
    return file_type::regular_file;
  else if (S_ISBLK(Mode))
    return file_type::block_file;
  else if (S_ISCHR(Mode))
    return file_type::character_file;
  else if (S_ISFIFO(Mode))
    return file_type::fifo_file;
  else if (S_ISSOCK(Mode))
    return file_type::socket_file;
  else if (S_ISLNK(Mode))
    return file_type::symlink_file;
  return file_type::type_unknown;
}

static std::error_code fillStatus(int StatRet, const struct stat &Status,
                                  file_status &Result) {
  if (StatRet != 0) {
    std::error_code EC(errno, std::generic_category());
    if (EC == errc::no_such_file_or_directory)
      Result = file_status(file_type::file_not_found);
    else
      Result = file_status(file_type::status_error);
    return EC;
  }

  uint32_t atime_nsec, mtime_nsec;
#if defined(__APPLE__)
  atime_nsec = Status.st_atimespec.tv_nsec;
  mtime_nsec = Status.st_mtimespec.tv_nsec;
#else
  atime_nsec = Status.st_atim.tv_nsec;
  mtime_nsec = Status.st_mtim.tv_nsec;
#endif

  perms Perms = static_cast<perms>(Status.st_mode) & all_perms;
  Result = file_status(typeForMode(Status.st_mode), Perms, Status.st_dev,
                       Status.st_nlink, Status.st_ino,
                       Status.st_atime, atime_nsec, Status.st_mtime, mtime_nsec,
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

std::error_code mapped_file_region::init(int FD, uint64_t Offset,
                                         mapmode Mode) {
  assert(Size != 0);

  int flags = (Mode == readwrite) ? MAP_SHARED : MAP_PRIVATE;
  int prot = (Mode == readonly) ? PROT_READ : (PROT_READ | PROT_WRITE);
#if defined(__APPLE__)
  //----------------------------------------------------------------------
  // Newer versions of MacOSX have a flag that will allow us to read from
  // binaries whose code signature is invalid without crashing by using
  // the MAP_RESILIENT_CODESIGN flag. Also if a file from removable media
  // is mapped we can avoid crashing and return zeroes to any pages we try
  // to read if the media becomes unavailable by using the
  // MAP_RESILIENT_MEDIA flag.  These flags are only usable when mapping
  // with PROT_READ, so take care not to specify them otherwise.
  //----------------------------------------------------------------------
  if (Mode == readonly) {
#if defined(MAP_RESILIENT_CODESIGN)
    flags |= MAP_RESILIENT_CODESIGN;
#endif
#if defined(MAP_RESILIENT_MEDIA)
    flags |= MAP_RESILIENT_MEDIA;
#endif
  }
#endif // #if defined (__APPLE__)

  Mapping = ::mmap(nullptr, Size, prot, flags, FD, Offset);
  if (Mapping == MAP_FAILED)
    return std::error_code(errno, std::generic_category());
  return std::error_code();
}

mapped_file_region::mapped_file_region(int fd, mapmode mode, size_t length,
                                       uint64_t offset, std::error_code &ec)
    : Size(length), Mapping(), Mode(mode) {
  (void)Mode;
  ec = init(fd, offset, mode);
  if (ec)
    Mapping = nullptr;
}

mapped_file_region::~mapped_file_region() {
  if (Mapping)
    ::munmap(Mapping, Size);
}

size_t mapped_file_region::size() const {
  assert(Mapping && "Mapping failed but used anyway!");
  return Size;
}

char *mapped_file_region::data() const {
  assert(Mapping && "Mapping failed but used anyway!");
  return reinterpret_cast<char*>(Mapping);
}

const char *mapped_file_region::const_data() const {
  assert(Mapping && "Mapping failed but used anyway!");
  return reinterpret_cast<const char*>(Mapping);
}

int mapped_file_region::alignment() {
  return ::sysconf(_SC_PAGE_SIZE);
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

static file_type direntType(dirent* Entry) {
  // Most platforms provide the file type in the dirent: Linux/BSD/Mac.
  // The DTTOIF macro lets us reuse our status -> type conversion.
  return typeForMode(DTTOIF(Entry->d_type));
}

std::error_code detail::directory_iterator_increment(detail::DirIterState &It) {
  errno = 0;
  dirent *CurDir = ::readdir(reinterpret_cast<DIR *>(It.IterationHandle));
  if (CurDir == nullptr && errno != 0) {
    return std::error_code(errno, std::generic_category());
  } else if (CurDir != nullptr) {
    StringRef Name(CurDir->d_name);
    if ((Name.size() == 1 && Name[0] == '.') ||
        (Name.size() == 2 && Name[0] == '.' && Name[1] == '.'))
      return directory_iterator_increment(It);
    It.CurrentEntry.replace_filename(Name, direntType(CurDir));
  } else
    return directory_iterator_destruct(It);

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

static int nativeOpenFlags(CreationDisposition Disp, OpenFlags Flags,
                           FileAccess Access) {
  int Result = 0;
  if (Access == FA_Read)
    Result |= O_RDONLY;
  else if (Access == FA_Write)
    Result |= O_WRONLY;
  else if (Access == (FA_Read | FA_Write))
    Result |= O_RDWR;

  // This is for compatibility with old code that assumed F_Append implied
  // would open an existing file.  See Windows/Path.inc for a longer comment.
  if (Flags & F_Append)
    Disp = CD_OpenAlways;

  if (Disp == CD_CreateNew) {
    Result |= O_CREAT; // Create if it doesn't exist.
    Result |= O_EXCL;  // Fail if it does.
  } else if (Disp == CD_CreateAlways) {
    Result |= O_CREAT; // Create if it doesn't exist.
    Result |= O_TRUNC; // Truncate if it does.
  } else if (Disp == CD_OpenAlways) {
    Result |= O_CREAT; // Create if it doesn't exist.
  } else if (Disp == CD_OpenExisting) {
    // Nothing special, just don't add O_CREAT and we get these semantics.
  }

  if (Flags & F_Append)
    Result |= O_APPEND;

#ifdef O_CLOEXEC
  if (!(Flags & OF_ChildInherit))
    Result |= O_CLOEXEC;
#endif

  return Result;
}

std::error_code openFile(const Twine &Name, int &ResultFD,
                         CreationDisposition Disp, FileAccess Access,
                         OpenFlags Flags, unsigned Mode) {
  int OpenFlags = nativeOpenFlags(Disp, Flags, Access);

  SmallString<128> Storage;
  StringRef P = Name.toNullTerminatedStringRef(Storage);
  // Call ::open in a lambda to avoid overload resolution in RetryAfterSignal
  // when open is overloaded, such as in Bionic.
  auto Open = [&]() { return ::open(P.begin(), OpenFlags, Mode); };
  if ((ResultFD = sys::RetryAfterSignal(-1, Open)) < 0)
    return std::error_code(errno, std::generic_category());
#ifndef O_CLOEXEC
  if (!(Flags & OF_ChildInherit)) {
    int r = fcntl(ResultFD, F_SETFD, FD_CLOEXEC);
    (void)r;
    assert(r == 0 && "fcntl(F_SETFD, FD_CLOEXEC) failed");
  }
#endif
  return std::error_code();
}

Expected<int> openNativeFile(const Twine &Name, CreationDisposition Disp,
                             FileAccess Access, OpenFlags Flags,
                             unsigned Mode) {

  int FD;
  std::error_code EC = openFile(Name, FD, Disp, Access, Flags, Mode);
  if (EC)
    return errorCodeToError(EC);
  return FD;
}

std::error_code openFileForRead(const Twine &Name, int &ResultFD,
                                OpenFlags Flags,
                                SmallVectorImpl<char> *RealPath) {
  std::error_code EC =
      openFile(Name, ResultFD, CD_OpenExisting, FA_Read, Flags, 0666);
  if (EC)
    return EC;

  // Attempt to get the real name of the file, if the user asked
  if(!RealPath)
    return std::error_code();
  RealPath->clear();
#if defined(F_GETPATH)
  // When F_GETPATH is available, it is the quickest way to get
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
    SmallString<128> Storage;
    StringRef P = Name.toNullTerminatedStringRef(Storage);

    // Use ::realpath to get the real path name
    if (::realpath(P.begin(), Buffer) != nullptr)
      RealPath->append(Buffer, Buffer + strlen(Buffer));
  }
#endif
  return std::error_code();
}

Expected<file_t> openNativeFileForRead(const Twine &Name, OpenFlags Flags,
                                       SmallVectorImpl<char> *RealPath) {
  file_t ResultFD;
  std::error_code EC = openFileForRead(Name, ResultFD, Flags, RealPath);
  if (EC)
    return errorCodeToError(EC);
  return ResultFD;
}

void closeFile(file_t &F) {
  ::close(F);
  F = kInvalidFile;
}

} // end namespace fs

namespace path {

bool home_directory(SmallVectorImpl<char> &result) {
  char *RequestedDir = getenv("HOME");
  if (!RequestedDir) {
    struct passwd *pw = getpwuid(getuid());
    if (pw && pw->pw_dir)
      RequestedDir = pw->pw_dir;
  }
  if (!RequestedDir)
    return false;

  result.clear();
  result.append(RequestedDir, RequestedDir + strlen(RequestedDir));
  return true;
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

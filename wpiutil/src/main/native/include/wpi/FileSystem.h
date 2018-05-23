//===- llvm/Support/FileSystem.h - File System OS Concept -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the wpi::sys::fs namespace. It is designed after
// TR2/boost filesystem (v3), but modified to remove exception handling and the
// path class.
//
// All functions return an error_code and their actual work via the last out
// argument. The out argument is defined if and only if errc::success is
// returned. A function may return any error code in the generic or system
// category. However, they shall be equivalent to any error conditions listed
// in each functions respective documentation if the condition applies. [ note:
// this does not guarantee that error_code will be in the set of explicitly
// listed codes, but it does guarantee that if any of the explicitly listed
// errors occur, the correct error_code will be used ]. All functions may
// return errc::not_enough_memory if there is not enough memory to complete the
// operation.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_FILESYSTEM_H
#define WPIUTIL_WPI_FILESYSTEM_H

#include "wpi/SmallString.h"
#include "wpi/StringRef.h"
#include "wpi/Twine.h"
#include "wpi/ErrorOr.h"
#include <cassert>
#include <cstdint>
#include <ctime>
#include <memory>
#include <stack>
#include <string>
#include <system_error>
#include <tuple>
#include <vector>

#include <sys/stat.h>

namespace wpi {
namespace sys {
namespace fs {

/// An enumeration for the file system's view of the type.
enum class file_type {
  status_error,
  file_not_found,
  regular_file,
  directory_file,
  symlink_file,
  block_file,
  character_file,
  fifo_file,
  socket_file,
  type_unknown
};

enum perms {
  no_perms = 0,
  owner_read = 0400,
  owner_write = 0200,
  owner_exe = 0100,
  owner_all = owner_read | owner_write | owner_exe,
  group_read = 040,
  group_write = 020,
  group_exe = 010,
  group_all = group_read | group_write | group_exe,
  others_read = 04,
  others_write = 02,
  others_exe = 01,
  others_all = others_read | others_write | others_exe,
  all_read = owner_read | group_read | others_read,
  all_write = owner_write | group_write | others_write,
  all_exe = owner_exe | group_exe | others_exe,
  all_all = owner_all | group_all | others_all,
  set_uid_on_exe = 04000,
  set_gid_on_exe = 02000,
  sticky_bit = 01000,
  all_perms = all_all | set_uid_on_exe | set_gid_on_exe | sticky_bit,
  perms_not_known = 0xFFFF
};

// Helper functions so that you can use & and | to manipulate perms bits:
inline perms operator|(perms l, perms r) {
  return static_cast<perms>(static_cast<unsigned short>(l) |
                            static_cast<unsigned short>(r));
}
inline perms operator&(perms l, perms r) {
  return static_cast<perms>(static_cast<unsigned short>(l) &
                            static_cast<unsigned short>(r));
}
inline perms &operator|=(perms &l, perms r) {
  l = l | r;
  return l;
}
inline perms &operator&=(perms &l, perms r) {
  l = l & r;
  return l;
}
inline perms operator~(perms x) {
  // Avoid UB by explicitly truncating the (unsigned) ~ result.
  return static_cast<perms>(
      static_cast<unsigned short>(~static_cast<unsigned short>(x)));
}

class UniqueID {
  uint64_t Device;
  uint64_t File;

public:
  UniqueID() = default;
  UniqueID(uint64_t Device, uint64_t File) : Device(Device), File(File) {}

  bool operator==(const UniqueID &Other) const {
    return Device == Other.Device && File == Other.File;
  }
  bool operator!=(const UniqueID &Other) const { return !(*this == Other); }
  bool operator<(const UniqueID &Other) const {
    return std::tie(Device, File) < std::tie(Other.Device, Other.File);
  }

  uint64_t getDevice() const { return Device; }
  uint64_t getFile() const { return File; }
};

/// Represents the result of a call to directory_iterator::status(). This is a
/// subset of the information returned by a regular sys::fs::status() call, and
/// represents the information provided by Windows FileFirstFile/FindNextFile.
class basic_file_status {
protected:
  #ifndef _WIN32
  time_t fs_st_atime = 0;
  time_t fs_st_mtime = 0;
  uid_t fs_st_uid = 0;
  gid_t fs_st_gid = 0;
  off_t fs_st_size = 0;
  #else
  uint32_t LastAccessedTimeHigh = 0;
  uint32_t LastAccessedTimeLow = 0;
  uint32_t LastWriteTimeHigh = 0;
  uint32_t LastWriteTimeLow = 0;
  uint32_t FileSizeHigh = 0;
  uint32_t FileSizeLow = 0;
  #endif
  file_type Type = file_type::status_error;
  perms Perms = perms_not_known;

public:
  basic_file_status() = default;

  explicit basic_file_status(file_type Type) : Type(Type) {}

  #ifndef _WIN32
  basic_file_status(file_type Type, perms Perms, time_t ATime, time_t MTime,
                    uid_t UID, gid_t GID, off_t Size)
      : fs_st_atime(ATime), fs_st_mtime(MTime), fs_st_uid(UID), fs_st_gid(GID),
        fs_st_size(Size), Type(Type), Perms(Perms) {}
  #else
  basic_file_status(file_type Type, perms Perms, uint32_t LastAccessTimeHigh,
                    uint32_t LastAccessTimeLow, uint32_t LastWriteTimeHigh,
                    uint32_t LastWriteTimeLow, uint32_t FileSizeHigh,
                    uint32_t FileSizeLow)
      : LastAccessedTimeHigh(LastAccessTimeHigh),
        LastAccessedTimeLow(LastAccessTimeLow),
        LastWriteTimeHigh(LastWriteTimeHigh),
        LastWriteTimeLow(LastWriteTimeLow), FileSizeHigh(FileSizeHigh),
        FileSizeLow(FileSizeLow), Type(Type), Perms(Perms) {}
  #endif

  // getters
  file_type type() const { return Type; }
  perms permissions() const { return Perms; }

  #ifndef _WIN32
  uint32_t getUser() const { return fs_st_uid; }
  uint32_t getGroup() const { return fs_st_gid; }
  uint64_t getSize() const { return fs_st_size; }
  #else
  uint32_t getUser() const {
    return 9999; // Not applicable to Windows, so...
  }

  uint32_t getGroup() const {
    return 9999; // Not applicable to Windows, so...
  }

  uint64_t getSize() const {
    return (uint64_t(FileSizeHigh) << 32) + FileSizeLow;
  }
  #endif

  // setters
  void type(file_type v) { Type = v; }
  void permissions(perms p) { Perms = p; }
};

/// Represents the result of a call to sys::fs::status().
class file_status : public basic_file_status {
  friend bool equivalent(file_status A, file_status B);

  #ifndef _WIN32
  dev_t fs_st_dev = 0;
  nlink_t fs_st_nlinks = 0;
  ino_t fs_st_ino = 0;
  #else
  uint32_t NumLinks = 0;
  uint32_t VolumeSerialNumber = 0;
  uint32_t FileIndexHigh = 0;
  uint32_t FileIndexLow = 0;
  #endif

public:
  file_status() = default;

  explicit file_status(file_type Type) : basic_file_status(Type) {}

  #ifndef _WIN32
  file_status(file_type Type, perms Perms, dev_t Dev, nlink_t Links, ino_t Ino,
              time_t ATime, time_t MTime, uid_t UID, gid_t GID, off_t Size)
      : basic_file_status(Type, Perms, ATime, MTime, UID, GID, Size),
        fs_st_dev(Dev), fs_st_nlinks(Links), fs_st_ino(Ino) {}
  #else
  file_status(file_type Type, perms Perms, uint32_t LinkCount,
              uint32_t LastAccessTimeHigh, uint32_t LastAccessTimeLow,
              uint32_t LastWriteTimeHigh, uint32_t LastWriteTimeLow,
              uint32_t VolumeSerialNumber, uint32_t FileSizeHigh,
              uint32_t FileSizeLow, uint32_t FileIndexHigh,
              uint32_t FileIndexLow)
      : basic_file_status(Type, Perms, LastAccessTimeHigh, LastAccessTimeLow,
                          LastWriteTimeHigh, LastWriteTimeLow, FileSizeHigh,
                          FileSizeLow),
        NumLinks(LinkCount), VolumeSerialNumber(VolumeSerialNumber),
        FileIndexHigh(FileIndexHigh), FileIndexLow(FileIndexLow) {}
  #endif

  UniqueID getUniqueID() const;
  uint32_t getLinkCount() const;
};

/// @}
/// @name Physical Operators
/// @{

/// Make \a path an absolute path.
///
/// Makes \a path absolute using the \a current_directory if it is not already.
/// An empty \a path will result in the \a current_directory.
///
/// /absolute/path   => /absolute/path
/// relative/../path => <current-directory>/relative/../path
///
/// @param path A path that is modified to be an absolute path.
/// @returns errc::success if \a path has been made absolute, otherwise a
///          platform-specific error_code.
std::error_code make_absolute(const Twine &current_directory,
                              SmallVectorImpl<char> &path);

/// Make \a path an absolute path.
///
/// Makes \a path absolute using the current directory if it is not already. An
/// empty \a path will result in the current directory.
///
/// /absolute/path   => /absolute/path
/// relative/../path => <current-directory>/relative/../path
///
/// @param path A path that is modified to be an absolute path.
/// @returns errc::success if \a path has been made absolute, otherwise a
///          platform-specific error_code.
std::error_code make_absolute(SmallVectorImpl<char> &path);

/// Get the current path.
///
/// @param result Holds the current path on return.
/// @returns errc::success if the current path has been stored in result,
///          otherwise a platform-specific error_code.
std::error_code current_path(SmallVectorImpl<char> &result);

/// @}
/// @name Physical Observers
/// @{

/// Does file exist?
///
/// @param status A basic_file_status previously returned from stat.
/// @returns True if the file represented by status exists, false if it does
///          not.
bool exists(const basic_file_status &status);

enum class AccessMode { Exist, Write, Execute };

/// Can the file be accessed?
///
/// @param Path Input path.
/// @returns errc::success if the path can be accessed, otherwise a
///          platform-specific error_code.
std::error_code access(const Twine &Path, AccessMode Mode);

/// Does file exist?
///
/// @param Path Input path.
/// @returns True if it exists, false otherwise.
inline bool exists(const Twine &Path) {
  return !access(Path, AccessMode::Exist);
}

/// Can we write this file?
///
/// @param Path Input path.
/// @returns True if we can write to it, false otherwise.
inline bool can_write(const Twine &Path) {
  return !access(Path, AccessMode::Write);
}

/// Do file_status's represent the same thing?
///
/// @param A Input file_status.
/// @param B Input file_status.
///
/// assert(status_known(A) || status_known(B));
///
/// @returns True if A and B both represent the same file system entity, false
///          otherwise.
bool equivalent(file_status A, file_status B);

/// Do paths represent the same thing?
///
/// assert(status_known(A) || status_known(B));
///
/// @param A Input path A.
/// @param B Input path B.
/// @param result Set to true if stat(A) and stat(B) have the same device and
///               inode (or equivalent).
/// @returns errc::success if result has been successfully set, otherwise a
///          platform-specific error_code.
std::error_code equivalent(const Twine &A, const Twine &B, bool &result);

/// Simpler version of equivalent for clients that don't need to
///        differentiate between an error and false.
inline bool equivalent(const Twine &A, const Twine &B) {
  bool result;
  return !equivalent(A, B, result) && result;
}

/// Does status represent a directory?
///
/// @param status A basic_file_status previously returned from status.
/// @returns status.type() == file_type::directory_file.
bool is_directory(const basic_file_status &status);

/// Is path a directory?
///
/// @param path Input path.
/// @param result Set to true if \a path is a directory (after following
///               symlinks, false if it is not. Undefined otherwise.
/// @returns errc::success if result has been successfully set, otherwise a
///          platform-specific error_code.
std::error_code is_directory(const Twine &path, bool &result);

/// Simpler version of is_directory for clients that don't need to
///        differentiate between an error and false.
inline bool is_directory(const Twine &Path) {
  bool Result;
  return !is_directory(Path, Result) && Result;
}

/// Does status represent a regular file?
///
/// @param status A basic_file_status previously returned from status.
/// @returns status_known(status) && status.type() == file_type::regular_file.
bool is_regular_file(const basic_file_status &status);

/// Is path a regular file?
///
/// @param path Input path.
/// @param result Set to true if \a path is a regular file (after following
///               symlinks), false if it is not. Undefined otherwise.
/// @returns errc::success if result has been successfully set, otherwise a
///          platform-specific error_code.
std::error_code is_regular_file(const Twine &path, bool &result);

/// Simpler version of is_regular_file for clients that don't need to
///        differentiate between an error and false.
inline bool is_regular_file(const Twine &Path) {
  bool Result;
  if (is_regular_file(Path, Result))
    return false;
  return Result;
}

/// Does status represent a symlink file?
///
/// @param status A basic_file_status previously returned from status.
/// @returns status_known(status) && status.type() == file_type::symlink_file.
bool is_symlink_file(const basic_file_status &status);

/// Is path a symlink file?
///
/// @param path Input path.
/// @param result Set to true if \a path is a symlink file, false if it is not.
///               Undefined otherwise.
/// @returns errc::success if result has been successfully set, otherwise a
///          platform-specific error_code.
std::error_code is_symlink_file(const Twine &path, bool &result);

/// Simpler version of is_symlink_file for clients that don't need to
///        differentiate between an error and false.
inline bool is_symlink_file(const Twine &Path) {
  bool Result;
  if (is_symlink_file(Path, Result))
    return false;
  return Result;
}

/// Does this status represent something that exists but is not a
///        directory or regular file?
///
/// @param status A basic_file_status previously returned from status.
/// @returns exists(s) && !is_regular_file(s) && !is_directory(s)
bool is_other(const basic_file_status &status);

/// Is path something that exists but is not a directory,
///        regular file, or symlink?
///
/// @param path Input path.
/// @param result Set to true if \a path exists, but is not a directory, regular
///               file, or a symlink, false if it does not. Undefined otherwise.
/// @returns errc::success if result has been successfully set, otherwise a
///          platform-specific error_code.
std::error_code is_other(const Twine &path, bool &result);

/// Get file status as if by POSIX stat().
///
/// @param path Input path.
/// @param result Set to the file status.
/// @param follow When true, follows symlinks.  Otherwise, the symlink itself is
///               statted.
/// @returns errc::success if result has been successfully set, otherwise a
///          platform-specific error_code.
std::error_code status(const Twine &path, file_status &result,
                       bool follow = true);

/// A version for when a file descriptor is already available.
std::error_code status(int FD, file_status &Result);

/// Is status available?
///
/// @param s Input file status.
/// @returns True if status() != status_error.
bool status_known(const basic_file_status &s);

/// Is status available?
///
/// @param path Input path.
/// @param result Set to true if status() != status_error.
/// @returns errc::success if result has been successfully set, otherwise a
///          platform-specific error_code.
std::error_code status_known(const Twine &path, bool &result);

enum OpenFlags : unsigned {
  F_None = 0,

  /// F_Excl - When opening a file, this flag makes raw_fd_ostream
  /// report an error if the file already exists.
  F_Excl = 1,

  /// F_Append - When opening a file, if it already exists append to the
  /// existing file instead of returning an error.  This may not be specified
  /// with F_Excl.
  F_Append = 2,

  /// F_NoTrunc - When opening a file, if it already exists don't truncate
  /// the file contents.  F_Append implies F_NoTrunc, but F_Append seeks to
  /// the end of the file, which F_NoTrunc doesn't.
  F_NoTrunc = 4,

  /// The file should be opened in text mode on platforms that make this
  /// distinction.
  F_Text = 8,

  /// Open the file for read and write.
  F_RW = 16,

  /// Delete the file on close. Only makes a difference on windows.
  F_Delete = 32
};

inline OpenFlags operator|(OpenFlags A, OpenFlags B) {
  return OpenFlags(unsigned(A) | unsigned(B));
}

inline OpenFlags &operator|=(OpenFlags &A, OpenFlags B) {
  A = A | B;
  return A;
}

/// @brief Opens the file with the given name in a write-only or read-write
/// mode, returning its open file descriptor. If the file does not exist, it
/// is created.
///
/// The caller is responsible for closing the file descriptor once they are
/// finished with it.
///
/// @param Name The path of the file to open, relative or absolute.
/// @param ResultFD If the file could be opened successfully, its descriptor
///                 is stored in this location. Otherwise, this is set to -1.
/// @param Flags Additional flags used to determine whether the file should be
///              opened in, for example, read-write or in write-only mode.
/// @param Mode The access permissions of the file, represented in octal.
/// @returns errc::success if \a Name has been opened, otherwise a
///          platform-specific error_code.
std::error_code openFileForWrite(const Twine &Name, int &ResultFD,
                                 OpenFlags Flags, unsigned Mode = 0666);

/// @brief Opens the file with the given name in a read-only mode, returning
/// its open file descriptor.
///
/// The caller is responsible for closing the file descriptor once they are
/// finished with it.
///
/// @param Name The path of the file to open, relative or absolute.
/// @param ResultFD If the file could be opened successfully, its descriptor
///                 is stored in this location. Otherwise, this is set to -1.
/// @param RealPath If nonnull, extra work is done to determine the real path
///                 of the opened file, and that path is stored in this
///                 location.
/// @returns errc::success if \a Name has been opened, otherwise a
///          platform-specific error_code.
std::error_code openFileForRead(const Twine &Name, int &ResultFD,
                                SmallVectorImpl<char> *RealPath = nullptr);

std::error_code getUniqueID(const Twine Path, UniqueID &Result);

/// @}
/// @name Iterators
/// @{

/// directory_entry - A single entry in a directory. Caches the status either
/// from the result of the iteration syscall, or the first time status is
/// called.
class directory_entry {
  std::string Path;
  bool FollowSymlinks;
  basic_file_status Status;

public:
  explicit directory_entry(const Twine &path, bool follow_symlinks = true,
                           basic_file_status st = basic_file_status())
      : Path(path.str()), FollowSymlinks(follow_symlinks), Status(st) {}

  directory_entry() = default;

  void assign(const Twine &path, basic_file_status st = basic_file_status()) {
    Path = path.str();
    Status = st;
  }

  void replace_filename(const Twine &filename,
                        basic_file_status st = basic_file_status());

  const std::string &path() const { return Path; }
  ErrorOr<basic_file_status> status() const;

  bool operator==(const directory_entry& rhs) const { return Path == rhs.Path; }
  bool operator!=(const directory_entry& rhs) const { return !(*this == rhs); }
  bool operator< (const directory_entry& rhs) const;
  bool operator<=(const directory_entry& rhs) const;
  bool operator> (const directory_entry& rhs) const;
  bool operator>=(const directory_entry& rhs) const;
};

namespace detail {

  struct DirIterState;

  std::error_code directory_iterator_construct(DirIterState &, StringRef, bool);
  std::error_code directory_iterator_increment(DirIterState &);
  std::error_code directory_iterator_destruct(DirIterState &);

  /// Keeps state for the directory_iterator.
  struct DirIterState {
    ~DirIterState() {
      directory_iterator_destruct(*this);
    }

    intptr_t IterationHandle = 0;
    directory_entry CurrentEntry;
  };

} // end namespace detail

/// directory_iterator - Iterates through the entries in path. There is no
/// operator++ because we need an error_code. If it's really needed we can make
/// it call report_fatal_error on error.
class directory_iterator {
  std::shared_ptr<detail::DirIterState> State;
  bool FollowSymlinks = true;

public:
  explicit directory_iterator(const Twine &path, std::error_code &ec,
                              bool follow_symlinks = true)
      : FollowSymlinks(follow_symlinks) {
    State = std::make_shared<detail::DirIterState>();
    SmallString<128> path_storage;
    ec = detail::directory_iterator_construct(
        *State, path.toStringRef(path_storage), FollowSymlinks);
    update_error_code_for_current_entry(ec);
  }

  explicit directory_iterator(const directory_entry &de, std::error_code &ec,
                              bool follow_symlinks = true)
      : FollowSymlinks(follow_symlinks) {
    State = std::make_shared<detail::DirIterState>();
    ec = detail::directory_iterator_construct(
        *State, de.path(), FollowSymlinks);
    update_error_code_for_current_entry(ec);
  }

  /// Construct end iterator.
  directory_iterator() = default;

  // No operator++ because we need error_code.
  directory_iterator &increment(std::error_code &ec) {
    ec = directory_iterator_increment(*State);
    update_error_code_for_current_entry(ec);
    return *this;
  }

  const directory_entry &operator*() const { return State->CurrentEntry; }
  const directory_entry *operator->() const { return &State->CurrentEntry; }

  bool operator==(const directory_iterator &RHS) const {
    if (State == RHS.State)
      return true;
    if (!RHS.State)
      return State->CurrentEntry == directory_entry();
    if (!State)
      return RHS.State->CurrentEntry == directory_entry();
    return State->CurrentEntry == RHS.State->CurrentEntry;
  }

  bool operator!=(const directory_iterator &RHS) const {
    return !(*this == RHS);
  }
  // Other members as required by
  // C++ Std, 24.1.1 Input iterators [input.iterators]

private:
  // Checks if current entry is valid and populates error code. For example,
  // current entry may not exist due to broken symbol links.
  void update_error_code_for_current_entry(std::error_code &ec) {
    // Bail out if error has already occured earlier to avoid overwriting it.
    if (ec)
      return;

    // Empty directory entry is used to mark the end of an interation, it's not
    // an error.
    if (State->CurrentEntry == directory_entry())
      return;

    ErrorOr<basic_file_status> status = State->CurrentEntry.status();
    if (!status)
      ec = status.getError();
  }
};

namespace detail {

  /// Keeps state for the recursive_directory_iterator.
  struct RecDirIterState {
    std::stack<directory_iterator, std::vector<directory_iterator>> Stack;
    uint16_t Level = 0;
    bool HasNoPushRequest = false;
  };

} // end namespace detail

/// recursive_directory_iterator - Same as directory_iterator except for it
/// recurses down into child directories.
class recursive_directory_iterator {
  std::shared_ptr<detail::RecDirIterState> State;
  bool Follow;

public:
  recursive_directory_iterator() = default;
  explicit recursive_directory_iterator(const Twine &path, std::error_code &ec,
                                        bool follow_symlinks = true)
      : State(std::make_shared<detail::RecDirIterState>()),
        Follow(follow_symlinks) {
    State->Stack.push(directory_iterator(path, ec, Follow));
    if (State->Stack.top() == directory_iterator())
      State.reset();
  }

  // No operator++ because we need error_code.
  recursive_directory_iterator &increment(std::error_code &ec) {
    const directory_iterator end_itr = {};

    if (State->HasNoPushRequest)
      State->HasNoPushRequest = false;
    else {
      ErrorOr<basic_file_status> status = State->Stack.top()->status();
      if (status && is_directory(*status)) {
        State->Stack.push(directory_iterator(*State->Stack.top(), ec, Follow));
        if (State->Stack.top() != end_itr) {
          ++State->Level;
          return *this;
        }
        State->Stack.pop();
      }
    }

    while (!State->Stack.empty()
           && State->Stack.top().increment(ec) == end_itr) {
      State->Stack.pop();
      --State->Level;
    }

    // Check if we are done. If so, create an end iterator.
    if (State->Stack.empty())
      State.reset();

    return *this;
  }

  const directory_entry &operator*() const { return *State->Stack.top(); }
  const directory_entry *operator->() const { return &*State->Stack.top(); }

  // observers
  /// Gets the current level. Starting path is at level 0.
  int level() const { return State->Level; }

  /// Returns true if no_push has been called for this directory_entry.
  bool no_push_request() const { return State->HasNoPushRequest; }

  // modifiers
  /// Goes up one level if Level > 0.
  void pop() {
    assert(State && "Cannot pop an end iterator!");
    assert(State->Level > 0 && "Cannot pop an iterator with level < 1");

    const directory_iterator end_itr = {};
    std::error_code ec;
    do {
      if (ec) {
        //report_fatal_error("Error incrementing directory iterator.");
        while (!State->Stack.empty()) State->Stack.pop();
        break;
      }
      State->Stack.pop();
      --State->Level;
    } while (!State->Stack.empty()
             && State->Stack.top().increment(ec) == end_itr);

    // Check if we are done. If so, create an end iterator.
    if (State->Stack.empty())
      State.reset();
  }

  /// Does not go down into the current directory_entry.
  void no_push() { State->HasNoPushRequest = true; }

  bool operator==(const recursive_directory_iterator &RHS) const {
    return State == RHS.State;
  }

  bool operator!=(const recursive_directory_iterator &RHS) const {
    return !(*this == RHS);
  }
  // Other members as required by
  // C++ Std, 24.1.1 Input iterators [input.iterators]
};

/// @}

} // end namespace fs
} // end namespace sys
} // end namespace wpi

#endif // LLVM_SUPPORT_FILESYSTEM_H

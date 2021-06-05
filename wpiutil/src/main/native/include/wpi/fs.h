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

#pragma once

#include <string_view>
#include <system_error>

#if defined(__APPLE__)
#include <Availability.h>
#endif
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) \
     || (defined(__cplusplus) && __cplusplus >= 201703L)) \
    && defined(__has_include)
#if __has_include(<filesystem>) \
    && (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) \
        || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500) \
    && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ >= 10 \
        || (__GNUC__ >= 9 && __GNUC_MINOR__ >= 1))
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs {
using namespace std::filesystem;
using ifstream = std::ifstream;
using ofstream = std::ofstream;
using fstream = std::fstream;
}  // namespace fs
#endif
#endif
#ifndef GHC_USE_STD_FS
// #define GHC_WIN_DISABLE_WSTRING_STORAGE_TYPE
#define GHC_FILESYSTEM_FWD
#include "ghc/filesystem.hpp"
namespace fs {
using namespace ghc::filesystem;
using ifstream = ghc::filesystem::ifstream;
using ofstream = ghc::filesystem::ofstream;
using fstream = ghc::filesystem::fstream;
}  // namespace fs
#endif

namespace fs {

#if defined(_WIN32)
// A Win32 HANDLE is a typedef of void*
using file_t = void*;
#else
using file_t = int;
#endif

extern const file_t kInvalidFile;

enum CreationDisposition : unsigned {
  /// CD_CreateAlways - When opening a file:
  ///   * If it already exists, truncate it.
  ///   * If it does not already exist, create a new file.
  CD_CreateAlways = 0,

  /// CD_CreateNew - When opening a file:
  ///   * If it already exists, fail.
  ///   * If it does not already exist, create a new file.
  CD_CreateNew = 1,

  /// CD_OpenExisting - When opening a file:
  ///   * If it already exists, open the file with the offset set to 0.
  ///   * If it does not already exist, fail.
  CD_OpenExisting = 2,

  /// CD_OpenAlways - When opening a file:
  ///   * If it already exists, open the file with the offset set to 0.
  ///   * If it does not already exist, create a new file.
  CD_OpenAlways = 3,
};

enum FileAccess : unsigned {
  FA_Read = 1,
  FA_Write = 2,
};

enum OpenFlags : unsigned {
  OF_None = 0,
  F_None = 0,  // For compatibility

  /// The file should be opened in text mode on platforms that make this
  /// distinction.
  OF_Text = 1,
  F_Text = 1,  // For compatibility

  /// The file should be opened in append mode.
  OF_Append = 2,
  F_Append = 2,  // For compatibility

  /// Delete the file on close. Only makes a difference on windows.
  OF_Delete = 4,

  /// When a child process is launched, this file should remain open in the
  /// child process.
  OF_ChildInherit = 8,

  /// Force files Atime to be updated on access. Only makes a difference on
  /// windows.
  OF_UpdateAtime = 16,
};

inline OpenFlags operator|(OpenFlags A, OpenFlags B) {
  return OpenFlags(unsigned(A) | unsigned(B));
}

inline OpenFlags& operator|=(OpenFlags& A, OpenFlags B) {
  A = A | B;
  return A;
}

inline FileAccess operator|(FileAccess A, FileAccess B) {
  return FileAccess(unsigned(A) | unsigned(B));
}

inline FileAccess& operator|=(FileAccess& A, FileAccess B) {
  A = A | B;
  return A;
}

/**
 * Opens a file with the specified creation disposition, access mode,
 * and flags and returns a platform-specific file object.
 *
 * The caller is responsible for closing the file object once they are
 * finished with it.
 *
 * @param Path The path of the file to open, relative or absolute.
 * @param EC Error code output, set to non-zero on error
 * @param Disp Value specifying the existing-file behavior
 * @param Access Value specifying whether to open the file in read, write, or
 *               read-write mode.
 * @param Flags Additional flags.
 * @param Mode The access permissions of the file, represented in octal.
 * @returns errc::success if \a Name has been opened, otherwise a
 *          platform-specific error_code.
 */
file_t OpenFile(const path& Path, std::error_code& EC, CreationDisposition Disp,
                FileAccess Access, OpenFlags Flags, unsigned Mode = 0666);

/**
 * @brief Opens the file with the given name in a write-only or read-write
 * mode, returning its open file descriptor. If the file does not exist, it
 * is created.
 *
 * The caller is responsible for closing the freeing the file once they are
 * finished with it.
 *
 * @param Path The path of the file to open, relative or absolute.
 * @param EC Error code output, set to non-zero on error
 * @param Disp Value specifying the existing-file behavior
 * @param Flags Additional flags used to determine whether the file should be
 *              opened in, for example, read-write or in write-only mode.
 * @param Mode The access permissions of the file, represented in octal.
 * @returns a platform-specific file descriptor if \a Name has been opened,
 *          otherwise kInvalidFile.
 */
inline file_t OpenFileForWrite(const path& Path, std::error_code& EC,
                               CreationDisposition Disp, OpenFlags Flags,
                               unsigned Mode = 0666) {
  return OpenFile(Path, EC, Disp, FA_Write, Flags, Mode);
}

/**
 * @brief Opens the file with the given name in a write-only or read-write
 * mode, returning its open file descriptor. If the file does not exist, it
 * is created.
 *
 * The caller is responsible for closing the freeing the file once they are
 * finished with it.
 *
 * @param Path The path of the file to open, relative or absolute.
 * @param EC Error code output, set to non-zero on error
 * @param Disp Value specifying the existing-file behavior
 * @param Flags Additional flags used to determine whether the file should be
 *              opened in, for example, read-write or in write-only mode.
 * @param Mode The access permissions of the file, represented in octal.
 * @return a platform-specific file descriptor if \a Name has been opened,
 *         otherwise kInvalidFile.
 */
inline file_t OpenFileForReadWrite(const path& Path, std::error_code& EC,
                                   CreationDisposition Disp, OpenFlags Flags,
                                   unsigned Mode = 0666) {
  return OpenFile(Path, EC, Disp, FA_Write | FA_Read, Flags, Mode);
}

/**
 * Opens the file with the given name in a read-only mode, returning
 * its open file descriptor.
 *
 * The caller is responsible for closing the freeing the file once they are
 * finished with it.
 *
 * @param Path The path of the file to open, relative or absolute.
 * @param EC Error code output, set to non-zero on error
 * @param Flags Additional flags
 * @return a platform-specific file descriptor if \a Name has been opened,
 *         otherwise kInvalidFile.
 */
file_t OpenFileForRead(const path& Path, std::error_code& EC,
                       OpenFlags Flags = OF_None);

/**
 * Converts a file object to a file descriptor.  The returned file descriptor
 * must be closed with ::close() instead of CloseFile().
 *
 * @param F On input, this is the file to convert to a file descriptor.
 *          On output, the file is set to kInvalidFile.
 * @param EC Error code output, set to non-zero on error
 * @param Flags Flags passed to the OpenFile function that created file_t
 * @return file descriptor, or -1 on error
 */
int FileToFd(file_t& F, std::error_code& EC, OpenFlags Flags);

/**
 * Closes the file object.
 *
 * @param F On input, this is the file to close.  On output, the file is
 *          set to kInvalidFile.
 */
void CloseFile(file_t& F);

}  // namespace fs

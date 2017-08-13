//===- llvm/Support/FileSystem.h - File System OS Concept -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the llvm::sys::fs namespace. It is designed after
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

#ifndef LLVM_SUPPORT_FILESYSTEM_H
#define LLVM_SUPPORT_FILESYSTEM_H

#include "llvm/Twine.h"
#include <system_error>

namespace llvm {

template <typename T> class SmallVectorImpl;

namespace sys {
namespace fs {

/// @name Physical Observers
/// @{

enum OpenFlags : unsigned {
  F_None = 0,

  /// F_Excl - When opening a file, this flag makes raw_fd_ostream
  /// report an error if the file already exists.
  F_Excl = 1,

  /// F_Append - When opening a file, if it already exists append to the
  /// existing file instead of returning an error.  This may not be specified
  /// with F_Excl.
  F_Append = 2,

  /// The file should be opened in text mode on platforms that make this
  /// distinction.
  F_Text = 4,

  /// Open the file for read and write.
  F_RW = 8
};

inline OpenFlags operator|(OpenFlags A, OpenFlags B) {
  return OpenFlags(unsigned(A) | unsigned(B));
}

inline OpenFlags &operator|=(OpenFlags &A, OpenFlags B) {
  A = A | B;
  return A;
}

std::error_code openFileForWrite(const Twine &Name, int &ResultFD,
                                 OpenFlags Flags, unsigned Mode = 0666);

std::error_code openFileForRead(const Twine &Name, int &ResultFD,
                                SmallVectorImpl<char> *RealPath = nullptr);

/// @}

} // end namespace fs
} // end namespace sys
} // end namespace llvm

#endif // LLVM_SUPPORT_FILESYSTEM_H

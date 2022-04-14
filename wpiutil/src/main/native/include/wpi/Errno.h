//===- llvm/Support/Errno.h - Portable+convenient errno handling -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares some portable and convenient functions to deal with errno.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_ERRNO_H
#define WPIUTIL_WPI_ERRNO_H

#include <cerrno>
#include <string>
#include <type_traits>

namespace wpi {
namespace sys {

template <typename FailT, typename Fun, typename... Args>
inline auto RetryAfterSignal(const FailT &Fail, const Fun &F,
                             const Args &... As) -> decltype(F(As...)) {
  decltype(F(As...)) Res;
  do {
    errno = 0;
    Res = F(As...);
  } while (Res == Fail && errno == EINTR);
  return Res;
}

}  // namespace sys
}  // namespace wpi

#endif  // WPIUTIL_WPI_ERRNO_H

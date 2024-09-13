//===- llvm/Support/Errno.h - Portable+convenient errno handling -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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

namespace wpi {
namespace sys {

template <typename FailT, typename Fun, typename... Args>
inline decltype(auto) RetryAfterSignal(const FailT &Fail, const Fun &F,
                                       const Args &... As) {
  decltype(F(As...)) Res;
  do {
    errno = 0;
    Res = F(As...);
  } while (Res == Fail && errno == EINTR);
  return Res;
}

}  // namespace sys
}  // namespace wpi

#endif // WPIUTIL_WPI_ERRNO_H

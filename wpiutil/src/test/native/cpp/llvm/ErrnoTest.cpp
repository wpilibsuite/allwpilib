//===- ErrnoTest.cpp - Error handling unit tests --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/util/Errno.hpp"
#include "gtest/gtest.h"

using namespace wpi::util::sys;

TEST(ErrnoTest, RetryAfterSignal) {
  EXPECT_EQ(1, RetryAfterSignal(-1, [] { return 1; }));

  EXPECT_EQ(-1, RetryAfterSignal(-1, [] {
    errno = EAGAIN;
    return -1;
  }));
  EXPECT_EQ(EAGAIN, errno);

  unsigned calls = 0;
  EXPECT_EQ(1, RetryAfterSignal(-1, [&calls] {
              errno = EINTR;
              ++calls;
              return calls == 1 ? -1 : 1;
            }));
  EXPECT_EQ(2u, calls);

  EXPECT_EQ(1, RetryAfterSignal(-1, [](int x) { return x; }, 1));

  std::unique_ptr<int> P(RetryAfterSignal(nullptr, [] { return new int(47); }));
  EXPECT_EQ(47, *P);

  errno = EINTR;
  EXPECT_EQ(-1, RetryAfterSignal(-1, [] { return -1; }));
}

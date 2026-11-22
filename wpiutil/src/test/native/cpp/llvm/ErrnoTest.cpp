//===- ErrnoTest.cpp - Error handling unit tests --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/util/Errno.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>

using namespace wpi::util::sys;

TEST_CASE("ErrnoTest RetryAfterSignal", "[wpiutil][llvm]") {
  CHECK(1 == RetryAfterSignal(-1, [] { return 1; }));

  CHECK(-1 == RetryAfterSignal(-1, [] {
    errno = EAGAIN;
    return -1;
  }));
  CHECK(EAGAIN == errno);

  unsigned calls = 0;
  CHECK(1 == RetryAfterSignal(-1, [&calls] {
              errno = EINTR;
              ++calls;
              return calls == 1 ? -1 : 1;
            }));
  CHECK(2u == calls);

  CHECK(1 == RetryAfterSignal(-1, [](int x) { return x; }, 1));

  std::unique_ptr<int> P(RetryAfterSignal(nullptr, [] { return new int(47); }));
  CHECK(47 == *P);

  errno = EINTR;
  CHECK(-1 == RetryAfterSignal(-1, [] { return -1; }));
}

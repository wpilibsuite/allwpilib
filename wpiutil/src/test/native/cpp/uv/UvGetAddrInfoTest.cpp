/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "wpi/uv/GetAddrInfo.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include "wpi/uv/Loop.h"

#define CONCURRENT_COUNT 10

namespace wpi {
namespace uv {

TEST(UvGetAddrInfo, BothNull) {
  int fail_cb_called = 0;

  auto loop = Loop::Create();
  loop->error.connect([&](Error err) {
    ASSERT_EQ(err.code(), UV_EINVAL);
    fail_cb_called++;
  });

  GetAddrInfo(loop, [](const addrinfo&) { FAIL(); }, Twine::createNull());
  loop->Run();
  ASSERT_EQ(fail_cb_called, 1);
}

TEST(UvGetAddrInfo, FailedLookup) {
  int fail_cb_called = 0;

  auto loop = Loop::Create();
  loop->error.connect([&](Error err) {
    ASSERT_EQ(fail_cb_called, 0);
    ASSERT_LT(err.code(), 0);
    fail_cb_called++;
  });

  // Use a FQDN by ending in a period
  GetAddrInfo(loop, [](const addrinfo&) { FAIL(); }, "xyzzy.xyzzy.xyzzy.");
  loop->Run();
  ASSERT_EQ(fail_cb_called, 1);
}

TEST(UvGetAddrInfo, Basic) {
  int getaddrinfo_cbs = 0;

  auto loop = Loop::Create();
  loop->error.connect([](Error) { FAIL(); });

  GetAddrInfo(loop, [&](const addrinfo&) { getaddrinfo_cbs++; }, "localhost");

  loop->Run();

  ASSERT_EQ(getaddrinfo_cbs, 1);
}

#ifndef _WIN32
TEST(UvGetAddrInfo, Concurrent) {
  int getaddrinfo_cbs = 0;
  int callback_counts[CONCURRENT_COUNT];

  auto loop = Loop::Create();
  loop->error.connect([](Error) { FAIL(); });

  for (int i = 0; i < CONCURRENT_COUNT; i++) {
    callback_counts[i] = 0;
    GetAddrInfo(loop,
                [i, &callback_counts, &getaddrinfo_cbs](const addrinfo&) {
                  callback_counts[i]++;
                  getaddrinfo_cbs++;
                },
                "localhost");
  }

  loop->Run();

  for (int i = 0; i < CONCURRENT_COUNT; i++) {
    ASSERT_EQ(callback_counts[i], 1);
  }
}
#endif

}  // namespace uv
}  // namespace wpi

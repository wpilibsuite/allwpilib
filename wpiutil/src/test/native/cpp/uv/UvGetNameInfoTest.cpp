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

#include "wpi/uv/GetNameInfo.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

TEST(UvGetNameInfo, BasicIp4) {
  int getnameinfo_cbs = 0;

  auto loop = Loop::Create();
  loop->error.connect([](Error) { FAIL(); });

  GetNameInfo4(loop,
               [&](const char* hostname, const char* service) {
                 ASSERT_NE(hostname, nullptr);
                 ASSERT_NE(service, nullptr);
                 getnameinfo_cbs++;
               },
               "127.0.0.1", 80);

  loop->Run();

  ASSERT_EQ(getnameinfo_cbs, 1);
}

TEST(UvGetNameInfo, BasicIp6) {
  int getnameinfo_cbs = 0;

  auto loop = Loop::Create();
  loop->error.connect([](Error) { FAIL(); });

  GetNameInfo6(loop,
               [&](const char* hostname, const char* service) {
                 ASSERT_NE(hostname, nullptr);
                 ASSERT_NE(service, nullptr);
                 getnameinfo_cbs++;
               },
               "::1", 80);

  loop->Run();

  ASSERT_EQ(getnameinfo_cbs, 1);
}

}  // namespace uv
}  // namespace wpi

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

#include "wpi/uv/Loop.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include "wpi/uv/Timer.h"

namespace wpi {
namespace uv {

TEST(UvLoop, Walk) {
  int seen_timer_handle = 0;

  auto loop = Loop::Create();
  auto timer = Timer::Create(loop);

  loop->error.connect([](Error) { FAIL(); });

  timer->error.connect([](Error) { FAIL(); });

  timer->timeout.connect([&, theTimer = timer.get() ] {
    theTimer->GetLoopRef().Walk([&](Handle& it) {
      if (&it == timer.get()) seen_timer_handle++;
    });
    theTimer->Close();
  });
  timer->Start(Timer::Time{1});

  // Start event loop, expect to see the timer handle
  ASSERT_EQ(seen_timer_handle, 0);
  loop->Run();
  ASSERT_EQ(seen_timer_handle, 1);

  // Loop is finished, should not see our timer handle
  seen_timer_handle = 0;
  loop->Walk([&](Handle& it) {
    if (&it == timer.get()) seen_timer_handle++;
  });
  ASSERT_EQ(seen_timer_handle, 0);
}

}  // namespace uv
}  // namespace wpi

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

#include "wpi/uv/Async.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include <atomic>
#include <thread>

#include "wpi/mutex.h"
#include "wpi/uv/Loop.h"
#include "wpi/uv/Prepare.h"

namespace wpi {
namespace uv {

TEST(UvAsync, Test) {
  std::atomic_int async_cb_called{0};
  int prepare_cb_called = 0;
  int close_cb_called = 0;

  wpi::mutex mutex;
  mutex.lock();

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = Async<>::Create(loop);
  auto prepare = Prepare::Create(loop);

  loop->error.connect([](Error) { FAIL(); });

  prepare->error.connect([](Error) { FAIL(); });
  prepare->closed.connect([&] { close_cb_called++; });
  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] {
      for (;;) {
        mutex.lock();
        int n = async_cb_called;
        mutex.unlock();

        if (n == 3) {
          break;
        }

        async->Send();

        std::this_thread::yield();
      }
    });
    mutex.unlock();
  });
  prepare->Start();

  async->error.connect([](Error) { FAIL(); });
  async->closed.connect([&] { close_cb_called++; });
  async->wakeup.connect([&] {
    mutex.lock();
    int n = ++async_cb_called;
    mutex.unlock();

    if (n == 3) {
      async->Close();
      prepare->Close();
    }
  });

  loop->Run();

  ASSERT_GT(prepare_cb_called, 0);
  ASSERT_EQ(async_cb_called, 3);
  ASSERT_EQ(close_cb_called, 2);

  if (theThread.joinable()) theThread.join();
}

TEST(UvAsync, Data) {
  int prepare_cb_called = 0;
  int async_cb_called[2] = {0, 0};
  int close_cb_called = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = Async<int, std::function<void(int)>>::Create(loop);
  auto prepare = Prepare::Create(loop);

  loop->error.connect([](Error) { FAIL(); });

  prepare->error.connect([](Error) { FAIL(); });
  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] {
      async->Send(0, [&](int v) {
        ASSERT_EQ(v, 0);
        ++async_cb_called[0];
      });
      async->Send(1, [&](int v) {
        ASSERT_EQ(v, 1);
        ++async_cb_called[1];
        async->Close();
        prepare->Close();
      });
    });
  });
  prepare->Start();

  async->error.connect([](Error) { FAIL(); });
  async->closed.connect([&] { close_cb_called++; });
  async->wakeup.connect([&](int v, std::function<void(int)> f) { f(v); });

  loop->Run();

  ASSERT_EQ(async_cb_called[0], 1);
  ASSERT_EQ(async_cb_called[1], 1);
  ASSERT_EQ(close_cb_called, 1);

  if (theThread.joinable()) theThread.join();
}

TEST(UvAsync, DataRef) {
  int prepare_cb_called = 0;
  int val = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = Async<int, int&>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] { async->Send(1, val); });
  });
  prepare->Start();

  async->wakeup.connect([&](int v, int& r) {
    r = v;
    async->Close();
    prepare->Close();
  });

  loop->Run();

  ASSERT_EQ(val, 1);

  if (theThread.joinable()) theThread.join();
}

}  // namespace uv
}  // namespace wpi

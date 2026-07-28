// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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

// clang-format off
#include "wpi/net/uv/Async.hpp"
// clang-format on

#include <atomic>
#include <functional>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Prepare.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::net::uv {

TEST_CASE("UvAsyncTest CallbackOnly", "[uv][async]") {
  std::atomic_int async_cb_called{0};
  int prepare_cb_called = 0;
  int close_cb_called = 0;
  std::atomic_bool fail{false};

  wpi::util::mutex mutex;
  mutex.lock();

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = Async<>::Create(loop);
  auto prepare = Prepare::Create(loop);

  loop->error.connect([&](Error) { fail = true; });

  prepare->error.connect([&](Error) { fail = true; });
  prepare->closed.connect([&] { close_cb_called++; });
  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
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

  async->error.connect([&](Error) { fail = true; });
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

  if (fail) {
    FAIL();
  }

  REQUIRE(prepare_cb_called > 0);
  REQUIRE(async_cb_called == 3);
  REQUIRE(close_cb_called == 2);

  if (theThread.joinable()) {
    theThread.join();
  }
}

TEST_CASE("UvAsyncTest Data", "[uv][async]") {
  int prepare_cb_called = 0;
  int async_cb_called[2] = {0, 0};
  int close_cb_called = 0;
  std::atomic_bool fail{false};
  std::atomic_bool v0_check{false};
  std::atomic_bool v1_check{false};

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = Async<int, std::function<void(int)>>::Create(loop);
  auto prepare = Prepare::Create(loop);

  loop->error.connect([&](Error) { fail = true; });

  prepare->error.connect([&](Error) { fail = true; });
  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] {
      async->Send(0, [&](int v) {
        v0_check = v == 0;
        ++async_cb_called[0];
      });
      async->Send(1, [&](int v) {
        v1_check = v == 1;
        ++async_cb_called[1];
        async->Close();
        prepare->Close();
      });
    });
  });
  prepare->Start();

  async->error.connect([&](Error) { fail = true; });
  async->closed.connect([&] { close_cb_called++; });
  async->wakeup.connect([&](int v, std::function<void(int)> f) { f(v); });

  loop->Run();

  if (fail) {
    FAIL();
  }

  REQUIRE(async_cb_called[0] == 1);
  REQUIRE(async_cb_called[1] == 1);
  REQUIRE(close_cb_called == 1);

  if (theThread.joinable()) {
    theThread.join();
  }
  REQUIRE(v0_check);
  REQUIRE(v1_check);
}

TEST_CASE("UvAsyncTest DataRef", "[uv][async]") {
  int prepare_cb_called = 0;
  int val = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = Async<int, int&>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] { async->Send(1, val); });
  });
  prepare->Start();

  async->wakeup.connect([&](int v, int& r) {
    r = v;
    async->Close();
    prepare->Close();
  });

  loop->Run();

  REQUIRE(val == 1);

  if (theThread.joinable()) {
    theThread.join();
  }
}

}  // namespace wpi::net::uv

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/AsyncFunction.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include <thread>

#include "wpi/uv/Loop.h"
#include "wpi/uv/Prepare.h"

namespace wpi {
namespace uv {

TEST(UvAsyncFunction, Test) {
  int prepare_cb_called = 0;
  int async_cb_called[2] = {0, 0};
  int close_cb_called = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<int(int)>::Create(loop);
  auto prepare = Prepare::Create(loop);

  loop->error.connect([](Error) { FAIL(); });

  prepare->error.connect([](Error) { FAIL(); });
  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] {
      ASSERT_EQ(async->Call(0), 1);
      ASSERT_EQ(async->Call(1), 2);
    });
  });
  prepare->Start();

  async->error.connect([](Error) { FAIL(); });
  async->closed.connect([&] { close_cb_called++; });
  async->wakeup = [&](int v) {
    ++async_cb_called[v];
    if (v == 1) {
      async->Close();
      prepare->Close();
    }
    return v + 1;
  };

  loop->Run();

  ASSERT_EQ(async_cb_called[0], 1);
  ASSERT_EQ(async_cb_called[1], 1);
  ASSERT_EQ(close_cb_called, 1);

  if (theThread.joinable()) theThread.join();
}

TEST(UvAsyncFunction, Ref) {
  int prepare_cb_called = 0;
  int val = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<int(int, int&)>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] { ASSERT_EQ(async->Call(1, val), 2); });
  });
  prepare->Start();

  async->wakeup = [&](int v, int& r) {
    r = v;
    async->Close();
    prepare->Close();
    return v + 1;
  };

  loop->Run();

  ASSERT_EQ(val, 1);

  if (theThread.joinable()) theThread.join();
}

TEST(UvAsyncFunction, Movable) {
  int prepare_cb_called = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async =
      AsyncFunction<std::unique_ptr<int>(std::unique_ptr<int>)>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] {
      auto val = std::make_unique<int>(1);
      auto val2 = async->Call(std::move(val));
      ASSERT_NE(val2, nullptr);
      ASSERT_EQ(*val2, 1);
    });
  });
  prepare->Start();

  async->wakeup = [&](std::unique_ptr<int> v) {
    async->Close();
    prepare->Close();
    return v;
  };

  loop->Run();

  if (theThread.joinable()) theThread.join();
}

TEST(UvAsyncFunction, Send) {
  int prepare_cb_called = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async =
      AsyncFunction<std::unique_ptr<int>(std::unique_ptr<int>)>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] { async->Send(std::make_unique<int>(1)); });
  });
  prepare->Start();

  async->wakeup = [&](std::unique_ptr<int> v) {
    async->Close();
    prepare->Close();
    return v;
  };

  loop->Run();

  if (theThread.joinable()) theThread.join();
}

TEST(UvAsyncFunction, VoidCall) {
  int prepare_cb_called = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<void()>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] { async->Call(); });
  });
  prepare->Start();

  async->wakeup = [&]() {
    async->Close();
    prepare->Close();
  };

  loop->Run();

  if (theThread.joinable()) theThread.join();
}

}  // namespace uv
}  // namespace wpi

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/uv/AsyncFunction.hpp"  // NOLINT(build/include_order)

#include <memory>
#include <thread>
#include <utility>

#include <gtest/gtest.h>

#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Prepare.hpp"

namespace wpi::uv {

TEST(UvAsyncFunctionTest, Basic) {
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
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] {
      auto call0 = async->Call(0);
      auto call1 = async->Call(1);
      ASSERT_EQ(call0.get(), 1);
      ASSERT_EQ(call1.get(), 2);
    });
  });
  prepare->Start();

  async->error.connect([](Error) { FAIL(); });
  async->closed.connect([&] { close_cb_called++; });
  async->wakeup = [&](promise<int> out, int v) {
    ++async_cb_called[v];
    if (v == 1) {
      async->Close();
      prepare->Close();
    }
    out.set_value(v + 1);
  };

  loop->Run();

  ASSERT_EQ(async_cb_called[0], 1);
  ASSERT_EQ(async_cb_called[1], 1);
  ASSERT_EQ(close_cb_called, 1);

  if (theThread.joinable()) {
    theThread.join();
  }
}

TEST(UvAsyncFunctionTest, Ref) {
  int prepare_cb_called = 0;
  int val = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<int(int, int&)>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] { ASSERT_EQ(async->Call(1, val).get(), 2); });
  });
  prepare->Start();

  async->wakeup = [&](promise<int> out, int v, int& r) {
    r = v;
    async->Close();
    prepare->Close();
    out.set_value(v + 1);
  };

  loop->Run();

  ASSERT_EQ(val, 1);

  if (theThread.joinable()) {
    theThread.join();
  }
}

TEST(UvAsyncFunctionTest, Movable) {
  int prepare_cb_called = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async =
      AsyncFunction<std::unique_ptr<int>(std::unique_ptr<int>)>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] {
      auto val = std::make_unique<int>(1);
      auto val2 = async->Call(std::move(val)).get();
      ASSERT_NE(val2, nullptr);
      ASSERT_EQ(*val2, 1);
    });
  });
  prepare->Start();

  async->wakeup = [&](promise<std::unique_ptr<int>> out,
                      std::unique_ptr<int> v) {
    async->Close();
    prepare->Close();
    out.set_value(std::move(v));
  };

  loop->Run();

  if (theThread.joinable()) {
    theThread.join();
  }
}

TEST(UvAsyncFunctionTest, CallIgnoreResult) {
  int prepare_cb_called = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async =
      AsyncFunction<std::unique_ptr<int>(std::unique_ptr<int>)>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] { async->Call(std::make_unique<int>(1)); });
  });
  prepare->Start();

  async->wakeup = [&](promise<std::unique_ptr<int>> out,
                      std::unique_ptr<int> v) {
    async->Close();
    prepare->Close();
    out.set_value(std::move(v));
  };

  loop->Run();

  if (theThread.joinable()) {
    theThread.join();
  }
}

TEST(UvAsyncFunctionTest, VoidCall) {
  int prepare_cb_called = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<void()>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] { async->Call(); });
  });
  prepare->Start();

  async->wakeup = [&](promise<void> out) {
    async->Close();
    prepare->Close();
    out.set_value();
  };

  loop->Run();

  if (theThread.joinable()) {
    theThread.join();
  }
}

TEST(UvAsyncFunctionTest, WaitFor) {
  int prepare_cb_called = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<int()>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] {
      ASSERT_FALSE(async->Call().wait_for(std::chrono::milliseconds(10)));
    });
  });
  prepare->Start();

  async->wakeup = [&](promise<int> out) {
    async->Close();
    prepare->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    out.set_value(1);
  };

  loop->Run();

  if (theThread.joinable()) {
    theThread.join();
  }
}

TEST(UvAsyncFunctionTest, VoidWaitFor) {
  int prepare_cb_called = 0;

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<void()>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] {
      ASSERT_FALSE(async->Call().wait_for(std::chrono::milliseconds(10)));
    });
  });
  prepare->Start();

  async->wakeup = [&](promise<void> out) {
    async->Close();
    prepare->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    out.set_value();
  };

  loop->Run();

  if (theThread.joinable()) {
    theThread.join();
  }
}

}  // namespace wpi::uv

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// clang-format off
#include "wpi/net/uv/AsyncFunction.hpp"
// clang-format on

#include <atomic>
#include <memory>
#include <thread>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Prepare.hpp"

namespace wpi::net::uv {

TEST_CASE("UvAsyncFunctionTest Basic", "[uv][async-function]") {
  int prepare_cb_called = 0;
  int async_cb_called[2] = {0, 0};
  int close_cb_called = 0;
  std::atomic_bool fail{false};
  std::atomic_bool call0_check{false};
  std::atomic_bool call1_check{false};

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<int(int)>::Create(loop);
  auto prepare = Prepare::Create(loop);

  loop->error.connect([&](Error) { fail = true; });

  prepare->error.connect([&](Error) { fail = true; });
  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] {
      auto call0 = async->Call(0);
      auto call1 = async->Call(1);
      call0_check = call0.get() == 1;
      call1_check = call1.get() == 2;
    });
  });
  prepare->Start();

  async->error.connect([&](Error) { fail = true; });
  async->closed.connect([&] { close_cb_called++; });
  async->wakeup = [&](wpi::util::promise<int> out, int v) {
    ++async_cb_called[v];
    if (v == 1) {
      async->Close();
      prepare->Close();
    }
    out.set_value(v + 1);
  };

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
  REQUIRE(call0_check);
  REQUIRE(call1_check);
}

TEST_CASE("UvAsyncFunctionTest Ref", "[uv][async-function]") {
  int prepare_cb_called = 0;
  int val = 0;
  std::atomic_bool call_check{false};

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<int(int, int&)>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread =
        std::thread([&] { call_check = async->Call(1, val).get() == 2; });
  });
  prepare->Start();

  async->wakeup = [&](wpi::util::promise<int> out, int v, int& r) {
    r = v;
    async->Close();
    prepare->Close();
    out.set_value(v + 1);
  };

  loop->Run();

  REQUIRE(val == 1);

  if (theThread.joinable()) {
    theThread.join();
  }
  REQUIRE(call_check);
}

TEST_CASE("UvAsyncFunctionTest Movable", "[uv][async-function]") {
  int prepare_cb_called = 0;
  std::atomic_bool val2_check{false};

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
      val2_check = val2 != nullptr && *val2 == 1;
    });
  });
  prepare->Start();

  async->wakeup = [&](wpi::util::promise<std::unique_ptr<int>> out,
                      std::unique_ptr<int> v) {
    async->Close();
    prepare->Close();
    out.set_value(std::move(v));
  };

  loop->Run();

  if (theThread.joinable()) {
    theThread.join();
  }
  REQUIRE(val2_check);
}

TEST_CASE("UvAsyncFunctionTest CallIgnoreResult", "[uv][async-function]") {
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

  async->wakeup = [&](wpi::util::promise<std::unique_ptr<int>> out,
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

TEST_CASE("UvAsyncFunctionTest VoidCall", "[uv][async-function]") {
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

  async->wakeup = [&](wpi::util::promise<void> out) {
    async->Close();
    prepare->Close();
    out.set_value();
  };

  loop->Run();

  if (theThread.joinable()) {
    theThread.join();
  }
}

TEST_CASE("UvAsyncFunctionTest WaitFor", "[uv][async-function]") {
  int prepare_cb_called = 0;
  std::atomic_bool call_check{false};

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<int()>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] {
      call_check = !async->Call().wait_for(std::chrono::milliseconds(10));
    });
  });
  prepare->Start();

  async->wakeup = [&](wpi::util::promise<int> out) {
    async->Close();
    prepare->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    out.set_value(1);
  };

  loop->Run();

  if (theThread.joinable()) {
    theThread.join();
  }
  REQUIRE(call_check);
}

TEST_CASE("UvAsyncFunctionTest VoidWaitFor", "[uv][async-function]") {
  int prepare_cb_called = 0;
  std::atomic_bool call_check{false};

  std::thread theThread;

  auto loop = Loop::Create();
  auto async = AsyncFunction<void()>::Create(loop);
  auto prepare = Prepare::Create(loop);

  prepare->prepare.connect([&] {
    if (prepare_cb_called++) {
      return;
    }
    theThread = std::thread([&] {
      call_check = !async->Call().wait_for(std::chrono::milliseconds(10));
    });
  });
  prepare->Start();

  async->wakeup = [&](wpi::util::promise<void> out) {
    async->Close();
    prepare->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    out.set_value();
  };

  loop->Run();

  if (theThread.joinable()) {
    theThread.join();
  }
  REQUIRE(call_check);
}

}  // namespace wpi::net::uv

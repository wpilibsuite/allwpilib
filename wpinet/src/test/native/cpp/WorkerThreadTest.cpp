// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/WorkerThread.hpp"

#include <thread>

#include <gtest/gtest.h>

#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/net/uv/Loop.hpp"
#include "wpi/util/condition_variable.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::net {

TEST(WorkerThreadTest, Future) {
  WorkerThread<int(bool)> worker;
  wpi::util::future<int> f =
      worker.QueueWork([](bool v) -> int { return v ? 1 : 2; }, true);
  ASSERT_EQ(f.get(), 1);
}

TEST(WorkerThreadTest, FutureVoid) {
  int callbacks = 0;
  WorkerThread<void(int)> worker;
  wpi::util::future<void> f = worker.QueueWork(
      [&](int v) {
        ++callbacks;
        ASSERT_EQ(v, 3);
      },
      3);
  f.get();
  ASSERT_EQ(callbacks, 1);
}

TEST(WorkerThreadTest, Loop) {
  wpi::util::mutex m;
  wpi::util::condition_variable cv;
  int callbacks = 0;

  WorkerThread<int(bool)> worker;
  EventLoopRunner runner;
  runner.ExecSync([&](uv::Loop& loop) { worker.SetLoop(loop); });
  worker.QueueWorkThen([](bool v) -> int { return v ? 1 : 2; },
                       [&](int v2) {
                         std::scoped_lock lock{m};
                         ++callbacks;
                         cv.notify_all();
                         ASSERT_EQ(v2, 1);
                       },
                       true);
  auto f = worker.QueueWork([&](bool) -> int { return 2; }, true);
  ASSERT_EQ(f.get(), 2);

  std::unique_lock lock{m};
  cv.wait(lock, [&] { return callbacks == 1; });
  ASSERT_EQ(callbacks, 1);
}

TEST(WorkerThreadTest, LoopVoid) {
  wpi::util::mutex m;
  wpi::util::condition_variable cv;
  int callbacks = 0;

  WorkerThread<void(bool)> worker;
  EventLoopRunner runner;
  runner.ExecSync([&](uv::Loop& loop) { worker.SetLoop(loop); });
  worker.QueueWorkThen([](bool) {},
                       [&]() {
                         std::scoped_lock lock{m};
                         ++callbacks;
                         cv.notify_all();
                       },
                       true);
  auto f = worker.QueueWork([](bool) {}, true);
  f.get();

  std::unique_lock lock{m};
  cv.wait(lock, [&] { return callbacks == 1; });
  ASSERT_EQ(callbacks, 1);
}

}  // namespace wpi::net

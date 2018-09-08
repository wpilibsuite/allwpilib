/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/WorkerThread.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include <thread>

#include "wpi/uv/Loop.h"

namespace wpi {

TEST(WorkerThread, Future) {
  WorkerThread<int(bool)> worker;
  future<int> f =
      worker.QueueWork([](bool v) -> int { return v ? 1 : 2; }, true);
  ASSERT_EQ(f.get(), 1);
}

TEST(WorkerThread, FutureVoid) {
  int callbacks = 0;
  WorkerThread<void(int)> worker;
  future<void> f = worker.QueueWork(
      [&](int v) {
        ++callbacks;
        ASSERT_EQ(v, 3);
      },
      3);
  f.get();
  ASSERT_EQ(callbacks, 1);
}

TEST(WorkerThread, Loop) {
  int callbacks = 0;
  WorkerThread<int(bool)> worker;
  auto loop = uv::Loop::Create();
  worker.SetLoop(*loop);
  worker.QueueWorkThen([](bool v) -> int { return v ? 1 : 2; },
                       [&](int v2) {
                         ++callbacks;
                         loop->Stop();
                         ASSERT_EQ(v2, 1);
                       },
                       true);
  auto f = worker.QueueWork([](bool) -> int { return 2; }, true);
  ASSERT_EQ(f.get(), 2);
  loop->Run();
  ASSERT_EQ(callbacks, 1);
}

TEST(WorkerThread, LoopVoid) {
  int callbacks = 0;
  WorkerThread<void(bool)> worker;
  auto loop = uv::Loop::Create();
  worker.SetLoop(*loop);
  worker.QueueWorkThen([](bool) {},
                       [&]() {
                         ++callbacks;
                         loop->Stop();
                       },
                       true);
  auto f = worker.QueueWork([](bool) {}, true);
  f.get();
  loop->Run();
  ASSERT_EQ(callbacks, 1);
}

}  // namespace wpi

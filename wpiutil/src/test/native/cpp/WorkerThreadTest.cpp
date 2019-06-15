/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/WorkerThread.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include <thread>

namespace wpi {

TEST(WorkerThread, Test) {
  WorkerThread worker;
  auto exe = worker.GetExecutor();
  int cb_called[4] = {0, 0, 0, 0};
  auto outer_tid = std::this_thread::get_id();

  exe.execute([&] {
    ++cb_called[0];
    EXPECT_NE(std::this_thread::get_id(), outer_tid);
  });
  auto call0 = async(exe, [&] {
    ++cb_called[1];
    EXPECT_EQ(cb_called[0], 1);
    EXPECT_NE(std::this_thread::get_id(), outer_tid);
    return 1;
  });
  auto call1 = exe.twoway_execute([&] {
    ++cb_called[2];
    EXPECT_EQ(cb_called[0], 1);
    EXPECT_EQ(cb_called[1], 1);
    EXPECT_NE(std::this_thread::get_id(), outer_tid);
    return 2;
  });
  auto call2 = async(exe, [&] {
    ++cb_called[3];
    EXPECT_EQ(cb_called[0], 1);
    EXPECT_EQ(cb_called[1], 1);
    EXPECT_EQ(cb_called[2], 1);
    EXPECT_NE(std::this_thread::get_id(), outer_tid);
  });

  ASSERT_EQ(future_get(std::move(call0)), 1);
  ASSERT_EQ(future_get(std::move(call1)), 2);
  future_get(std::move(call2));

  ASSERT_EQ(cb_called[0], 1);
  ASSERT_EQ(cb_called[1], 1);
  ASSERT_EQ(cb_called[2], 1);
  ASSERT_EQ(cb_called[3], 1);
}

TEST(WorkerThread, TestThen) {
  int cb_called[4] = {0, 0, 0, 0};

  WorkerThread worker1;
  auto exe1 = worker1.GetExecutor();
  WorkerThread worker2;
  auto exe2 = worker2.GetExecutor();

  auto outer_tid = std::this_thread::get_id();
  std::thread::id thread1_tid;

  inline_executor imm_exe;
  auto call = async(imm_exe,
                    [&] {
                      ++cb_called[0];
                      EXPECT_EQ(std::this_thread::get_id(), outer_tid);
                      return 1;
                    })
                  .via(exe1)
                  .then([&](int v) {
                    ++cb_called[1];
                    thread1_tid = std::this_thread::get_id();
                    EXPECT_NE(thread1_tid, outer_tid);
                    EXPECT_EQ(v, 1);
                    return v + 1;
                  })
                  .then([&](int v) {
                    ++cb_called[2];
                    EXPECT_EQ(std::this_thread::get_id(), thread1_tid);
                    EXPECT_EQ(v, 2);
                  })
                  .via(exe2)
                  .then([&] {
                    ++cb_called[3];
                    EXPECT_NE(std::this_thread::get_id(), outer_tid);
                    EXPECT_NE(std::this_thread::get_id(), thread1_tid);
                  });
  future_get(std::move(call));

  ASSERT_EQ(cb_called[0], 1);
  ASSERT_EQ(cb_called[1], 1);
  ASSERT_EQ(cb_called[2], 1);
  ASSERT_EQ(cb_called[3], 1);
}

TEST(WorkerThread, ReadyThen) {
  int cb_called[4] = {0, 0, 0, 0};

  WorkerThread worker1;
  auto exe = worker1.GetExecutor();
  auto call =
      wpi::make_ready_future(1).via(exe).then([&](int v) { ++cb_called[0]; });
  future_get(std::move(call));

  ASSERT_EQ(cb_called[0], 1);
}

TEST(WorkerThread, Continuation) {
  int cb_called[4] = {0, 0, 0, 0};

  inline_executor exe;
  WorkerThread worker1, worker2;
  auto w1exe = worker1.GetExecutor();
  auto w2exe = worker2.GetExecutor();

  wpi::mutex m;
  wpi::condition_variable cv;
  bool ready = false;

  auto [inPromise, inFuture] = make_promise_contract<void>(exe);

  future<void> outFuture = std::move(inFuture)
                               .then([&] {
                                 ++cb_called[0];
                                 return wpi::async(w1exe, [&] {
                                   ++cb_called[1];
                                   std::unique_lock<wpi::mutex> lock(m);
                                   cv.wait(lock, [&] { return ready; });
                                   ++cb_called[2];
                                 });
                               })
                               .via(w2exe)
                               .then([&] { ++cb_called[3]; });

  std::move(inPromise).set_value();
  ASSERT_FALSE(outFuture.is_ready());
  ASSERT_EQ(cb_called[0], 1);

  {
    std::scoped_lock lock(m);
    ready = true;
  }
  cv.notify_one();

  wpi::future_get(std::move(outFuture));
  ASSERT_EQ(cb_called[1], 1);
  ASSERT_EQ(cb_called[2], 1);
  ASSERT_EQ(cb_called[3], 1);
}

}  // namespace wpi

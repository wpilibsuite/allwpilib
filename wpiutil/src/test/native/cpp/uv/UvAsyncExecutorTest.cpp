/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/AsyncExecutor.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include <thread>

#include "wpi/WorkerThread.h"
#include "wpi/uv/Loop.h"
#include "wpi/uv/Prepare.h"

namespace wpi {
namespace uv {

TEST(UvAsyncExecutor, Test) {
  int prepare_cb_called = 0;
  int async_cb_called[4] = {0, 0, 0, 0};

  std::thread theThread;

  auto loop = Loop::Create();
  auto exe = loop->GetExecutor();
  auto prepare = Prepare::Create(loop);
  auto loop_tid = std::this_thread::get_id();

  loop->error.connect([](Error e) { FAIL(); });

  prepare->error.connect([](Error) { FAIL(); });
  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] {
      exe.execute([&] {
        ++async_cb_called[0];
        EXPECT_EQ(std::this_thread::get_id(), loop_tid);
      });
      auto call0 = async(exe, [&] {
        ++async_cb_called[1];
        EXPECT_EQ(async_cb_called[0], 1);
        EXPECT_EQ(std::this_thread::get_id(), loop_tid);
        return 1;
      });
      auto call1 = exe.twoway_execute([&] {
        ++async_cb_called[2];
        EXPECT_EQ(async_cb_called[0], 1);
        EXPECT_EQ(async_cb_called[1], 1);
        EXPECT_EQ(std::this_thread::get_id(), loop_tid);
        return 2;
      });
      auto call2 = async(exe, [&] {
        ++async_cb_called[3];
        EXPECT_EQ(async_cb_called[0], 1);
        EXPECT_EQ(async_cb_called[1], 1);
        EXPECT_EQ(async_cb_called[2], 1);
        EXPECT_EQ(std::this_thread::get_id(), loop_tid);
      });
      exe.execute([&] { prepare->Close(); });
      ASSERT_EQ(future_get(std::move(call0)), 1);
      ASSERT_EQ(future_get(std::move(call1)), 2);
      future_get(std::move(call2));
    });
  });
  prepare->Start();

  loop->Run();

  ASSERT_GE(prepare_cb_called, 1);
  ASSERT_EQ(async_cb_called[0], 1);
  ASSERT_EQ(async_cb_called[1], 1);
  ASSERT_EQ(async_cb_called[2], 1);
  ASSERT_EQ(async_cb_called[3], 1);

  if (theThread.joinable()) theThread.join();
}

TEST(UvAsyncExecutor, TestThen) {
  int prepare_cb_called = 0;
  int async_cb_called[4] = {0, 0, 0, 0};

  std::thread theThread;

  auto loop = Loop::Create();
  auto exe = loop->GetExecutor();
  auto prepare = Prepare::Create(loop);
  auto loop_tid = std::this_thread::get_id();

  loop->error.connect([](Error e) { FAIL(); });

  prepare->error.connect([](Error) { FAIL(); });
  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] {
      auto thread_tid = std::this_thread::get_id();
      inline_executor imm_exe;
      auto call = async(imm_exe,
                        [&] {
                          ++async_cb_called[0];
                          EXPECT_EQ(std::this_thread::get_id(), thread_tid);
                          return 1;
                        })
                      .via(exe)
                      .then([&](int v) {
                        ++async_cb_called[1];
                        EXPECT_EQ(std::this_thread::get_id(), loop_tid);
                        EXPECT_EQ(v, 1);
                        return v + 1;
                      })
                      .then([&](int v) {
                        ++async_cb_called[2];
                        EXPECT_EQ(std::this_thread::get_id(), loop_tid);
                        EXPECT_EQ(v, 2);
                      })
                      .then([&] {
                        ++async_cb_called[3];
                        EXPECT_EQ(std::this_thread::get_id(), loop_tid);
                        prepare->Close();
                      });
      future_get(std::move(call));
    });
  });
  prepare->Start();

  loop->Run();

  ASSERT_GE(prepare_cb_called, 1);
  ASSERT_EQ(async_cb_called[0], 1);
  ASSERT_EQ(async_cb_called[1], 1);
  ASSERT_EQ(async_cb_called[2], 1);
  ASSERT_EQ(async_cb_called[3], 1);

  if (theThread.joinable()) theThread.join();
}

TEST(UvAsyncExecutor, TestWorker) {
  int prepare_cb_called = 0;
  int async_cb_called[4] = {0, 0, 0, 0};

  std::thread theThread;

  auto loop = Loop::Create();
  auto loopExe = loop->GetExecutor();
  WorkerThread worker;
  auto workerExe = worker.GetExecutor();

  auto prepare = Prepare::Create(loop);
  auto loop_tid = std::this_thread::get_id();

  loop->error.connect([](Error e) { FAIL(); });

  prepare->error.connect([](Error) { FAIL(); });
  prepare->prepare.connect([&] {
    if (prepare_cb_called++) return;
    theThread = std::thread([&] {
      auto call = async(loopExe,
                        [&] {
                          ++async_cb_called[0];
                          EXPECT_EQ(std::this_thread::get_id(), loop_tid);
                          return 1;
                        })
                      .via(workerExe)
                      .then([&](int v) {
                        ++async_cb_called[1];
                        EXPECT_NE(std::this_thread::get_id(), loop_tid);
                        EXPECT_EQ(v, 1);
                        return v + 1;
                      })
                      .via(loopExe)
                      .then([&](int v) {
                        ++async_cb_called[2];
                        EXPECT_EQ(std::this_thread::get_id(), loop_tid);
                        EXPECT_EQ(v, 2);
                      })
                      .then([&] {
                        ++async_cb_called[3];
                        EXPECT_EQ(std::this_thread::get_id(), loop_tid);
                        prepare->Close();
                      });
      future_get(std::move(call));
    });
  });
  prepare->Start();

  loop->Run();

  ASSERT_GE(prepare_cb_called, 1);
  ASSERT_EQ(async_cb_called[0], 1);
  ASSERT_EQ(async_cb_called[1], 1);
  ASSERT_EQ(async_cb_called[2], 1);
  ASSERT_EQ(async_cb_called[3], 1);

  if (theThread.joinable()) theThread.join();
}

}  // namespace uv
}  // namespace wpi

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Synchronization.hpp"

#include <array>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

TEST(EventTest, AutoReset) {
  auto event = wpi::util::MakeEvent(false, false);
  std::thread thr([&] { wpi::util::SetEvent(event); });
  wpi::util::WaitForObject(event);
  thr.join();
  bool timedOut;
  wpi::util::WaitForObject(event, 0, &timedOut);
  ASSERT_EQ(timedOut, true);
}

TEST(EventTest, ManualReset) {
  auto event = wpi::util::MakeEvent(true, false);
  int done = 0;
  std::thread thr([&] {
    wpi::util::SetEvent(event);
    ++done;
  });
  wpi::util::WaitForObject(event);
  thr.join();
  ASSERT_EQ(done, 1);
  bool timedOut;
  wpi::util::WaitForObject(event, 0, &timedOut);
  ASSERT_EQ(timedOut, false);
}

TEST(EventTest, InitialSet) {
  auto event = wpi::util::MakeEvent(false, true);
  bool timedOut;
  wpi::util::WaitForObject(event, 0, &timedOut);
  ASSERT_EQ(timedOut, false);
}

TEST(EventTest, WaitMultiple) {
  auto event1 = wpi::util::MakeEvent(false, false);
  auto event2 = wpi::util::MakeEvent(false, false);
  std::thread thr([&] { wpi::util::SetEvent(event2); });
  WPI_Handle signaled[2];
  auto result1 = wpi::util::WaitForObjects({event1, event2}, signaled);
  thr.join();
  ASSERT_EQ(result1.size(), 1u);
  ASSERT_EQ(result1[0], event2);
  bool timedOut;
  auto result2 =
      wpi::util::WaitForObjects({event1, event2}, signaled, 0, &timedOut);
  ASSERT_EQ(timedOut, true);
  ASSERT_EQ(result2.size(), 0u);
}

TEST(SignalObjectTest, ConcurrentSetDifferentHandles) {
  constexpr int HANDLE_COUNT = 64;
  constexpr int SET_COUNT = 100;

  std::array<WPI_Handle, HANDLE_COUNT> handles;
  for (int i = 0; i < HANDLE_COUNT; ++i) {
    handles[i] = (wpi::util::HANDLE_TYPE_USER_BASE << 24) | i;
    wpi::util::CreateSignalObject(handles[i], true, false);
  }

  std::vector<std::thread> threads;
  threads.reserve(HANDLE_COUNT);
  for (auto handle : handles) {
    threads.emplace_back([=] {
      for (int i = 0; i < SET_COUNT; ++i) {
        wpi::util::SetSignalObject(handle);
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  for (auto handle : handles) {
    bool timedOut = true;
    ASSERT_TRUE(wpi::util::WaitForObject(handle, 0, &timedOut));
    ASSERT_FALSE(timedOut);
    wpi::util::DestroySignalObject(handle);
  }
}

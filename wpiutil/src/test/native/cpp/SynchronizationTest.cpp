// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Synchronization.h"  // NOLINT(build/include_order)

#include <thread>

#include <gtest/gtest.h>

TEST(EventTest, AutoReset) {
  auto event = wpi::util::CreateEvent(false, false);
  std::thread thr([&] { wpi::util::SetEvent(event); });
  wpi::util::WaitForObject(event);
  thr.join();
  bool timedOut;
  wpi::util::WaitForObject(event, 0, &timedOut);
  ASSERT_EQ(timedOut, true);
}

TEST(EventTest, ManualReset) {
  auto event = wpi::util::CreateEvent(true, false);
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
  auto event = wpi::util::CreateEvent(false, true);
  bool timedOut;
  wpi::util::WaitForObject(event, 0, &timedOut);
  ASSERT_EQ(timedOut, false);
}

TEST(EventTest, WaitMultiple) {
  auto event1 = wpi::util::CreateEvent(false, false);
  auto event2 = wpi::util::CreateEvent(false, false);
  std::thread thr([&] { wpi::util::SetEvent(event2); });
  WPI_Handle signaled[2];
  auto result1 = wpi::util::WaitForObjects({event1, event2}, signaled);
  thr.join();
  ASSERT_EQ(result1.size(), 1u);
  ASSERT_EQ(result1[0], event2);
  bool timedOut;
  auto result2 = wpi::util::WaitForObjects({event1, event2}, signaled, 0, &timedOut);
  ASSERT_EQ(timedOut, true);
  ASSERT_EQ(result2.size(), 0u);
}

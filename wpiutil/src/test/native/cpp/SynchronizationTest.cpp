// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/Synchronization.h"  // NOLINT(build/include_order)

#include <thread>

#include "gtest/gtest.h"

TEST(EventTest, AutoReset) {
  auto event = wpi::CreateEvent(false, false);
  std::thread thr([&] { wpi::SetEvent(event); });
  wpi::WaitForObject(event);
  thr.join();
  bool timedOut;
  wpi::WaitForObject(event, 0, &timedOut);
  ASSERT_EQ(timedOut, true);
}

TEST(EventTest, ManualReset) {
  auto event = wpi::CreateEvent(true, false);
  int done = 0;
  std::thread thr([&] {
    wpi::SetEvent(event);
    ++done;
  });
  wpi::WaitForObject(event);
  thr.join();
  ASSERT_EQ(done, 1);
  bool timedOut;
  wpi::WaitForObject(event, 0, &timedOut);
  ASSERT_EQ(timedOut, false);
}

TEST(EventTest, InitialSet) {
  auto event = wpi::CreateEvent(false, true);
  bool timedOut;
  wpi::WaitForObject(event, 0, &timedOut);
  ASSERT_EQ(timedOut, false);
}

TEST(EventTest, WaitMultiple) {
  auto event1 = wpi::CreateEvent(false, false);
  auto event2 = wpi::CreateEvent(false, false);
  std::thread thr([&] { wpi::SetEvent(event2); });
  WPI_Handle signaled[2];
  auto result1 = wpi::WaitForObjects({event1, event2}, signaled);
  thr.join();
  ASSERT_EQ(result1.size(), 1u);
  ASSERT_EQ(result1[0], event2);
  bool timedOut;
  auto result2 = wpi::WaitForObjects({event1, event2}, signaled, 0, &timedOut);
  ASSERT_EQ(timedOut, true);
  ASSERT_EQ(result2.size(), 0u);
}

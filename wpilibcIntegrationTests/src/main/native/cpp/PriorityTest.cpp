// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <thread>

#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

#include "frc/Threads.h"
#include "gtest/gtest.h"

TEST(PriorityTest, SetThreadPriority) {
  bool exited = false;
  wpi::condition_variable cond;
  wpi::mutex mutex;
  std::thread thr{[&] {
    std::unique_lock lock{mutex};
    cond.wait(lock, [&] { return exited; });
  }};

  // Non-RT thread has priority of zero
  bool isRealTime = false;
  EXPECT_EQ(0, frc::GetThreadPriority(thr, &isRealTime));
  EXPECT_FALSE(isRealTime);

  // Set thread priority to 15
  EXPECT_TRUE(frc::SetThreadPriority(thr, true, 15));

  // Verify thread was given priority 15
  EXPECT_EQ(15, frc::GetThreadPriority(thr, &isRealTime));
  EXPECT_TRUE(isRealTime);

  // Set thread back to non-RT
  EXPECT_TRUE(frc::SetThreadPriority(thr, false, 0));

  // Verify thread is now non-RT
  EXPECT_EQ(0, frc::GetThreadPriority(thr, &isRealTime));
  EXPECT_FALSE(isRealTime);

  {
    std::scoped_lock lock{mutex};
    exited = true;
  }
  cond.notify_one();
  thr.join();
}

TEST(PriorityTest, SetCurrentThreadPriority) {
  // Non-RT thread has priority of zero
  bool isRealTime = true;
  EXPECT_EQ(0, frc::GetCurrentThreadPriority(&isRealTime));
  EXPECT_FALSE(isRealTime);

  // Set thread priority to 15
  EXPECT_TRUE(frc::SetCurrentThreadPriority(true, 15));

  // Verify thread was given priority 15
  EXPECT_EQ(15, frc::GetCurrentThreadPriority(&isRealTime));
  EXPECT_TRUE(isRealTime);

  // Set thread back to non-RT
  EXPECT_TRUE(frc::SetCurrentThreadPriority(false, 0));

  // Verify thread is now non-RT
  EXPECT_EQ(0, frc::GetCurrentThreadPriority(&isRealTime));
  EXPECT_FALSE(isRealTime);
}

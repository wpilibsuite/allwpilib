/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/FiberScheduler.h"  // NOLINT(build/include_order)

#include "frc/Fiber.h"  // NOLINT(build/include_order)

#include <stdint.h>

#include <thread>

#include <wpi/raw_ostream.h>

#include "gtest/gtest.h"

using namespace frc;

TEST(FiberSchedulerTest, AddRemove) {
  uint32_t fiberCounter = 0;

  FiberScheduler scheduler;
  Fiber fiber(0.4, [&] { fiberCounter++; });

  wpi::outs() << "Run 1\n";
  scheduler.Add(fiber);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  scheduler.Remove(fiber);

  EXPECT_EQ(0u, fiberCounter) << "Fiber triggered early";

  wpi::outs() << "Run 2\n";
  fiberCounter = 0;
  scheduler.Add(fiber);
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  scheduler.Remove(fiber);

  EXPECT_EQ(1u, fiberCounter)
      << "Fiber either didn't trigger or triggered more than once";

  wpi::outs() << "Run 3\n";
  fiberCounter = 0;
  scheduler.Add(fiber);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  scheduler.Remove(fiber);

  // Fiber scheduled every 400ms runs twice in 1000ms
  EXPECT_EQ(2u, fiberCounter)
      << "Fiber either didn't trigger or triggered more than once";
}

#ifdef __APPLE__
TEST(FiberSchedulerTest, DISABLED_MultiFiber) {
#else
TEST(FiberSchedulerTest, MultiFiber) {
#endif
  uint32_t fiberCounter1 = 0;
  uint32_t fiberCounter2 = 0;

  FiberScheduler scheduler;
  Fiber fiber1(0.2, [&] { fiberCounter1++; });
  Fiber fiber2(0.6, [&] { fiberCounter2++; });

  scheduler.Add(fiber2);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_EQ(0u, fiberCounter1) << "Fiber triggered early";
  EXPECT_EQ(0u, fiberCounter2) << "Fiber triggered early";

  // Sleep enough such that only the fiber enabled later times out first
  scheduler.Add(fiber1);
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  scheduler.Remove(fiber1);
  scheduler.Remove(fiber2);

  EXPECT_EQ(1u, fiberCounter1)
      << "Fiber either didn't trigger or triggered more than once";
  EXPECT_EQ(0u, fiberCounter2) << "Fiber triggered early";
}

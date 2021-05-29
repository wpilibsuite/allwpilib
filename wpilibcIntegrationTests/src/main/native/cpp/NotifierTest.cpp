// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Notifier.h"  // NOLINT(build/include_order)

#include <fmt/core.h>

#include "TestBench.h"
#include "frc/Timer.h"
#include "gtest/gtest.h"

unsigned notifierCounter;

void notifierHandler(void*) {
  notifierCounter++;
}

/**
 * Test if the Wait function works
 */
TEST(NotifierTest, DISABLED_TestTimerNotifications) {
  fmt::print("NotifierTest...\n");
  notifierCounter = 0;
  fmt::print("notifier(notifierHandler, nullptr)...\n");
  frc::Notifier notifier(notifierHandler, nullptr);
  fmt::print("Start Periodic...\n");
  notifier.StartPeriodic(1_s);

  fmt::print("Wait...\n");
  frc::Wait(10.5_s);
  fmt::print("...Wait\n");

  EXPECT_EQ(10u, notifierCounter)
      << "Received " << notifierCounter << " notifications in 10.5 seconds";
  fmt::print("Received {} notifications in 10.5 seconds", notifierCounter);

  fmt::print("...NotifierTest\n");
}

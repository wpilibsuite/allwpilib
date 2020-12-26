// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Notifier.h"  // NOLINT(build/include_order)

#include <wpi/raw_ostream.h>

#include "TestBench.h"
#include "frc/Timer.h"
#include "gtest/gtest.h"

using namespace frc;

unsigned notifierCounter;

void notifierHandler(void*) { notifierCounter++; }

/**
 * Test if the Wait function works
 */
TEST(NotifierTest, DISABLED_TestTimerNotifications) {
  wpi::outs() << "NotifierTest...\n";
  notifierCounter = 0;
  wpi::outs() << "notifier(notifierHandler, nullptr)...\n";
  Notifier notifier(notifierHandler, nullptr);
  wpi::outs() << "Start Periodic...\n";
  notifier.StartPeriodic(1.0);

  wpi::outs() << "Wait...\n";
  Wait(10.5);
  wpi::outs() << "...Wait\n";

  EXPECT_EQ(10u, notifierCounter)
      << "Received " << notifierCounter << " notifications in 10.5 seconds";
  wpi::outs() << "Received " << notifierCounter
              << " notifications in 10.5 seconds";

  wpi::outs() << "...NotifierTest\n";
}

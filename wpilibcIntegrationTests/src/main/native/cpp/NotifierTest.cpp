/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

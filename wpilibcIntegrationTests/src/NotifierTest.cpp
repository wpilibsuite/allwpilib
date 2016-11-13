/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Notifier.h"  // NOLINT(build/include_order)

#include "TestBench.h"
#include "Timer.h"
#include "gtest/gtest.h"
#include "llvm/raw_ostream.h"

using namespace frc;

unsigned notifierCounter;

void notifierHandler(void*) { notifierCounter++; }

/**
 * Test if the Wait function works
 */
TEST(NotifierTest, DISABLED_TestTimerNotifications) {
  llvm::outs() << "NotifierTest...\n";
  notifierCounter = 0;
  llvm::outs() << "notifier(notifierHandler, nullptr)...\n";
  Notifier notifier(notifierHandler, nullptr);
  llvm::outs() << "Start Periodic...\n";
  notifier.StartPeriodic(1.0);

  llvm::outs() << "Wait...\n";
  Wait(10.5);
  llvm::outs() << "...Wait\n";

  EXPECT_EQ(10u, notifierCounter) << "Received " << notifierCounter
                                  << " notifications in 10.5 seconds";
  llvm::outs() << "Received " << notifierCounter
               << " notifications in 10.5 seconds";

  llvm::outs() << "...NotifierTest\n";
}

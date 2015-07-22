/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <Notifier.h>
#include <Timer.h>
#include "gtest/gtest.h"
#include "TestBench.h"

unsigned notifierCounter;

void notifierHandler(void *) { notifierCounter++; }

/**
 * Test if the Wait function works
 */
TEST(NotifierTest, DISABLED_TestTimerNotifications) {
  std::cout << "NotifierTest..." << std::endl;
  notifierCounter = 0;
  std::cout << "notifier(notifierHandler, nullptr)..." << std::endl;
  Notifier notifier(notifierHandler, nullptr);
  std::cout << "Start Periodic..." << std::endl;
  notifier.StartPeriodic(1.0);

  std::cout << "Wait..." << std::endl;
  Wait(10.5);
  std::cout << "...Wait" << std::endl;

  EXPECT_EQ(10u, notifierCounter) << "Received " << notifierCounter
                                  << " notifications in 10.5 seconds";
  std::cout << "Received " << notifierCounter
            << " notifications in 10.5 seconds";

  std::cout << "...NotifierTest" << std::endl;
}

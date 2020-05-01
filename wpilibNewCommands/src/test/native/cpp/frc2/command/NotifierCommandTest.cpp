/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CommandTestBase.h"
#include "frc2/command/NotifierCommand.h"

using namespace frc2;
class NotifierCommandTest : public CommandTestBase {};

#ifdef __APPLE__
TEST_F(NotifierCommandTest, DISABLED_NotifierCommandScheduleTest) {
#else
TEST_F(NotifierCommandTest, NotifierCommandScheduleTest) {
#endif
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  NotifierCommand command([&counter] { counter++; }, 0.01_s, {});

  scheduler.Schedule(&command);
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  scheduler.Cancel(&command);

  EXPECT_GT(counter, 10);
  EXPECT_LT(counter, 100);
}

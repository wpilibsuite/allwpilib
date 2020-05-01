/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <regex>

#include "CommandTestBase.h"
#include "frc2/command/PrintCommand.h"

using namespace frc2;
class PrintCommandTest : public CommandTestBase {};

TEST_F(PrintCommandTest, PrintCommandScheduleTest) {
  CommandScheduler scheduler = GetScheduler();

  PrintCommand command("Test!");

  testing::internal::CaptureStdout();

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(std::regex_search(testing::internal::GetCapturedStdout(),
                                std::regex("Test!")));

  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

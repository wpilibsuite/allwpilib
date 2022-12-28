// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/TimedCommandRobot.h"

#include "frc2/command/CommandScheduler.h"

using namespace frc2;

TimedCommandRobot::TimedCommandRobot (units::second_t mainPeriod,
                           units::second_t schedulerPeriod)
    : frc::TimedRobot(mainPeriod) {
  AddPeriodic([] { CommandScheduler::GetInstance().Run(); }, schedulerPeriod,
              kSchedulerOffset);
}

TimedCommandRobot::TimedCommandRobot (units::second_t period) : frc::TimedRobot(period) {
  AddPeriodic([] { CommandScheduler::GetInstance().Run(); }, period,
              kSchedulerOffset);
}

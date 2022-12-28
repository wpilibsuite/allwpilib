// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/CommandRobot.h"

#include "frc2/command/CommandScheduler.h"

using namespace frc2;

CommandRobot::CommandRobot(units::second_t mainPeriod, units::second_t schedulerPeriod) : frc::TimedRobot(mainPeriod) {
    AddPeriodic([] { CommandScheduler::GetInstance().Run(); }, schedulerPeriod, kSchedulerOffset);
}

CommandRobot::CommandRobot(units::second_t period) {
    CommandRobot(period, period);
}

CommandRobot::CommandRobot() {
    CommandRobot(kDefaultPeriod);
}

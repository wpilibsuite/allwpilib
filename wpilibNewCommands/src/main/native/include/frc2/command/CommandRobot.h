// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/TimedRobot.h"
#include <frc2/command/CommandScheduler.h>
#include <frc2/command/button/Trigger.h>
#include <frc/DriverStation.h>

namespace frc2 {

class CommandRobot : public frc::TimedRobot {
    public:
    // explicit CommandRobot(units::second_t period = kDefaultPeriod) : TimedRobot(period) {}

    void RobotPeriodic() override;

    void SimulationPeriodic() override;

    void DisabledPeriodic() override;

    void AutonomousPeriodic() override;

    void TeleopPeriodic() override;

    void TestPeriodic() override;

    void TestInit() override;

    frc2::Trigger Autonomous();

    frc2::Trigger Teleop();

    frc2::Trigger Test();

    frc2::Trigger Simulation();
}

}  // namespace frc2

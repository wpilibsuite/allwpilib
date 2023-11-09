// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/CommandRobot.h"

using namespace frc2;

void CommandRobot::RobotPeriodic() {
    frc2::CommandScheduler::GetInstance().Run();
}

void CommandRobot::SimulationPeriodic() {}

void CommandRobot::DisabledPeriodic() {}

void CommandRobot::AutonomousPeriodic() {}

void CommandRobot::TeleopPeriodic() {}

void CommandRobot::TestPeriodic() {}

void CommandRobot::TestInit() {
    frc2::CommandScheduler::GetInstance().CancelAll();
}

frc2::Trigger CommandRobot::Autonomous() {
    return frc2::Trigger(frc::DriverStation::IsAutonomous);
}

frc2::Trigger CommandRobot::Teleop() {
    return frc2::Trigger(frc::DriverStation::IsTeleop);
}

frc2::Trigger CommandRobot::Test() {
    return frc2::Trigger(frc::DriverStation::IsTest);
}

frc2::Trigger CommandRobot::Simulation() {
    return frc2::Trigger(TimedRobot::IsSimulation);
}
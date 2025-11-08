// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/driverstation/Joystick.hpp>
#include <wpi/framework/TimedRobot.hpp>

#include "subsystems/Elevator.hpp"

/**
 * This is a sample program to demonstrate the use of elevator simulation.
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {}
  void RobotPeriodic() override;
  void SimulationPeriodic() override;
  void TeleopPeriodic() override;
  void DisabledInit() override;

 private:
  wpi::Joystick m_joystick{Constants::kJoystickPort};
  Elevator m_elevator;
};

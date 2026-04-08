// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "subsystems/Intake.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"

class Robot : public wpi::TimedRobot {
 public:
  void TeleopPeriodic() override;

 private:
  Intake m_intake;
  wpi::Joystick m_joystick{OperatorConstants::kJoystickIndex};
};

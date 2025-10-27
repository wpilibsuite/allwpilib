// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Joystick.h>
#include <frc/TimedRobot.h>

#include "Constants.h"
#include "subsystems/Intake.h"

class Robot : public frc::TimedRobot {
 public:
  void TeleopPeriodic() override;

 private:
  Intake m_intake;
  frc::Joystick m_joystick{OperatorConstants::kJoystickIndex};
};

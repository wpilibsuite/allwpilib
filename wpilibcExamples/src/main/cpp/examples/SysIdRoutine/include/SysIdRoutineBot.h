// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandPtr.h>
#include <frc2/command/button/CommandXboxController.h>

#include "Constants.h"
#include "subsystems/Drive.h"
#include "subsystems/Shooter.h"

class SysIdRoutineBot {
 public:
  SysIdRoutineBot();

  frc2::CommandPtr GetAutonomousCommand();

 private:
  void ConfigureBindings();
  frc2::CommandXboxController m_driverController{
      constants::oi::kDriverControllerPort};
  Drive m_drive;
  Shooter m_shooter;
};

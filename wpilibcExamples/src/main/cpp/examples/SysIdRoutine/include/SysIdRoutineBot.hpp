// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/commands2/CommandPtr.hpp>
#include <wpi/commands2/button/CommandXboxController.hpp>

#include "Constants.hpp"
#include "subsystems/Drive.hpp"
#include "subsystems/Shooter.hpp"

class SysIdRoutineBot {
 public:
  SysIdRoutineBot();

  wpi::cmd::CommandPtr GetAutonomousCommand();

 private:
  void ConfigureBindings();
  wpi::cmd::CommandXboxController m_driverController{
      constants::oi::kDriverControllerPort};
  Drive m_drive;
  Shooter m_shooter;
};

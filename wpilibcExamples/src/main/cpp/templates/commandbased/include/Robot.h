// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandPtr.h>
#include <frc2/command/CommandRobot.h>
#include <frc2/command/button/CommandXboxController.h>

#include "Constants.h"
#include "commands/Autos.h"
#include "subsystems/ExampleSubsystem.h"

class Robot : public frc2::CommandRobot {
 public:
  Robot();

 private:
  // Replace with CommandPS4Controller or CommandJoystick if needed
  frc2::CommandXboxController m_driverController{
      OperatorConstants::kDriverControllerPort};

  // The robot's subsystems are defined here.
  ExampleSubsystem m_subsystem;

  enum Autos { EXAMPLE_AUTO };

  frc::SendableChooser<Autos> m_autoChooser;
  frc2::CommandPtr m_exampleAuto{autos::ExampleAuto(&m_subsystem)};
};

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ExampleRobot.h"

#include "commands/Autos.h"

ExampleRobot::ExampleRobot() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureBindings();
}

void ExampleRobot::ConfigureBindings() {
  // Configure your button bindings here
}

frc2::CommandPtr ExampleRobot::GetAutonomousCommand() {
  // An example command will be run in autonomous
  return autos::ExampleAuto(&m_subsystem);
}

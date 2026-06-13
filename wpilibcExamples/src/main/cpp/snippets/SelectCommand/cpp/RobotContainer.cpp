// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include "wpi/smartdashboard/SmartDashboard.hpp"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  chooser.SetDefaultOption("ONE", CommandSelector::ONE);
  chooser.AddOption("TWO", CommandSelector::TWO);
  chooser.AddOption("THREE", CommandSelector::THREE);

  wpi::SmartDashboard::PutData("Auto Chooser", &chooser);

  // Configure the button bindings
  ConfigureButtonBindings();
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here
}

wpi::cmd::Command* RobotContainer::GetAutonomousCommand() {
  // Run the select command in autonomous
  return exampleSelectCommand.get();
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include "wpi/tunable/Tunables.hpp"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  chooser.AddDefault("ONE", CommandSelector::ONE);
  chooser.Add("TWO", CommandSelector::TWO);
  chooser.Add("THREE", CommandSelector::THREE);

  wpi::Tunables::Publish("Auto Chooser", chooser);

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

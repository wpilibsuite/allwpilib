// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include "wpi/smartdashboard/SmartDashboard.hpp"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  m_chooser.SetDefaultOption("ONE", CommandSelector::ONE);
  m_chooser.AddOption("TWO", CommandSelector::TWO);
  m_chooser.AddOption("THREE", CommandSelector::THREE);

  frc::SmartDashboard::PutData("Auto Chooser", &m_chooser);

  // Configure the button bindings
  ConfigureButtonBindings();
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // Run the select command in autonomous
  return m_exampleSelectCommand.get();
}

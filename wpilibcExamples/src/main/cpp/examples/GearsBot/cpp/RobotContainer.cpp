// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/smartdashboard/SmartDashboard.h>
#include <frc2/command/button/JoystickButton.h>

#include "commands/CloseClaw.h"
#include "commands/OpenClaw.h"
#include "commands/Pickup.h"
#include "commands/Place.h"
#include "commands/PrepareToPickup.h"
#include "commands/SetElevatorSetpoint.h"
#include "commands/TankDrive.h"

RobotContainer::RobotContainer()
    : m_autonomousCommand(&m_claw, &m_wrist, &m_elevator, &m_drivetrain) {
  frc::SmartDashboard::PutData(&m_drivetrain);
  frc::SmartDashboard::PutData(&m_elevator);
  frc::SmartDashboard::PutData(&m_wrist);
  frc::SmartDashboard::PutData(&m_claw);

  m_drivetrain.SetDefaultCommand(TankDrive([this] { return m_joy.GetLeftY(); },
                                           [this] { return m_joy.GetRightY(); },
                                           &m_drivetrain));

  // Configure the button bindings
  ConfigureButtonBindings();
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here
  frc2::JoystickButton(&m_joy, 5).WhenPressed(
      SetElevatorSetpoint(0.25, &m_elevator));
  frc2::JoystickButton(&m_joy, 6).WhenPressed(CloseClaw(&m_claw));
  frc2::JoystickButton(&m_joy, 7).WhenPressed(
      SetElevatorSetpoint(0.0, &m_elevator));
  frc2::JoystickButton(&m_joy, 8).WhenPressed(OpenClaw(&m_claw));
  frc2::JoystickButton(&m_joy, 9).WhenPressed(
      Autonomous(&m_claw, &m_wrist, &m_elevator, &m_drivetrain));
  frc2::JoystickButton(&m_joy, 10)
      .WhenPressed(Pickup(&m_claw, &m_wrist, &m_elevator));
  frc2::JoystickButton(&m_joy, 11)
      .WhenPressed(Place(&m_claw, &m_wrist, &m_elevator));
  frc2::JoystickButton(&m_joy, 12)
      .WhenPressed(PrepareToPickup(&m_claw, &m_wrist, &m_elevator));
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // An example command will be run in autonomous
  return &m_autonomousCommand;
}

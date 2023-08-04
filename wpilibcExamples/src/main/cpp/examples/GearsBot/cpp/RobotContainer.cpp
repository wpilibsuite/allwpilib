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
    : m_autonomousCommand(m_claw, m_wrist, m_elevator, m_drivetrain) {
  frc::SmartDashboard::PutData(&m_drivetrain);
  frc::SmartDashboard::PutData(&m_elevator);
  frc::SmartDashboard::PutData(&m_wrist);
  frc::SmartDashboard::PutData(&m_claw);

  m_drivetrain.SetDefaultCommand(
      TankDrive([this] { return -m_joy.GetLeftY(); },
                [this] { return -m_joy.GetRightY(); }, m_drivetrain));

  // Configure the button bindings
  ConfigureButtonBindings();
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here
  frc2::JoystickButton(&m_joy, 5).OnTrue(
      SetElevatorSetpoint(0.25, m_elevator).ToPtr());
  frc2::JoystickButton(&m_joy, 6).OnTrue(CloseClaw(m_claw).ToPtr());
  frc2::JoystickButton(&m_joy, 7).OnTrue(
      SetElevatorSetpoint(0.0, m_elevator).ToPtr());
  frc2::JoystickButton(&m_joy, 8).OnTrue(OpenClaw(m_claw).ToPtr());
  frc2::JoystickButton(&m_joy, 9).OnTrue(
      Autonomous(m_claw, m_wrist, m_elevator, m_drivetrain).ToPtr());
  frc2::JoystickButton(&m_joy, 10)
      .OnTrue(Pickup(m_claw, m_wrist, m_elevator).ToPtr());
  frc2::JoystickButton(&m_joy, 11)
      .OnTrue(Place(m_claw, m_wrist, m_elevator).ToPtr());
  frc2::JoystickButton(&m_joy, 12)
      .OnTrue(PrepareToPickup(m_claw, m_wrist, m_elevator).ToPtr());
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // An example command will be run in autonomous
  return &m_autonomousCommand;
}

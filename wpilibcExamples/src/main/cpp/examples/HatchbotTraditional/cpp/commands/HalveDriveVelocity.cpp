// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/HalveDriveVelocity.hpp"

HalveDriveVelocity::HalveDriveVelocity(DriveSubsystem* subsystem)
    : m_drive(subsystem) {}

void HalveDriveVelocity::Initialize() {
  m_drive->SetMaxOutput(0.5);
}

void HalveDriveVelocity::End(bool interrupted) {
  m_drive->SetMaxOutput(1);
}

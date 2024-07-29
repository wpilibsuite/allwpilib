// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/HalveDriveSpeed.h"

HalveDriveSpeed::HalveDriveSpeed(DriveSubsystem* subsystem)
    : m_drive(subsystem) {}

void HalveDriveSpeed::Initialize() {
  m_drive->SetMaxOutput(0.5);
}

void HalveDriveSpeed::End(bool interrupted) {
  m_drive->SetMaxOutput(1);
}

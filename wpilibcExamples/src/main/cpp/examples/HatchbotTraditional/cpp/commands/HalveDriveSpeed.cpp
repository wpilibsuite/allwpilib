/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/HalveDriveSpeed.h"

HalveDriveSpeed::HalveDriveSpeed(DriveSubsystem* subsystem)
    : m_drive(subsystem) {}

void HalveDriveSpeed::Initialize() { m_drive->SetMaxOutput(.5); }

void HalveDriveSpeed::End(bool interrupted) { m_drive->SetMaxOutput(1); }

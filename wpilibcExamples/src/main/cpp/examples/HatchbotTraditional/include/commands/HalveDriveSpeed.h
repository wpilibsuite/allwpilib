// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command2/Command.h>
#include <frc/command2/CommandHelper.h>

#include "subsystems/DriveSubsystem.h"

class HalveDriveSpeed
    : public frc::CommandHelper<frc::Command, HalveDriveSpeed> {
 public:
  explicit HalveDriveSpeed(DriveSubsystem* subsystem);

  void Initialize() override;

  void End(bool interrupted) override;

 private:
  DriveSubsystem* m_drive;
};

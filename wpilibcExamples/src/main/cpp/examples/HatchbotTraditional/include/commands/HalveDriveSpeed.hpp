// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/commands2/Command.hpp>
#include <wpi/commands2/CommandHelper.hpp>

#include "subsystems/DriveSubsystem.hpp"

class HalveDriveSpeed
    : public frc2::CommandHelper<frc2::Command, HalveDriveSpeed> {
 public:
  explicit HalveDriveSpeed(DriveSubsystem* subsystem);

  void Initialize() override;

  void End(bool interrupted) override;

 private:
  DriveSubsystem* m_drive;
};

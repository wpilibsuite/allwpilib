// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "subsystems/DriveSubsystem.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandHelper.hpp"

class HalveDriveVelocity
    : public wpi::cmd::CommandHelper<wpi::cmd::Command, HalveDriveVelocity> {
 public:
  explicit HalveDriveVelocity(DriveSubsystem* subsystem);

  void Initialize() override;

  void End(bool interrupted) override;

 private:
  DriveSubsystem* m_drive;
};

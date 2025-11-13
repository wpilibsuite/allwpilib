// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "subsystems/HatchSubsystem.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandHelper.hpp"

/**
 * A simple command that releases a hatch with the HatchSubsystem.  Written
 * explicitly for pedagogical purposes.  Actual code should inline a command
 * this simple with InstantCommand.
 *
 * @see InstantCommand
 */
class ReleaseHatch
    : public wpi::cmd::CommandHelper<wpi::cmd::Command, ReleaseHatch> {
 public:
  explicit ReleaseHatch(HatchSubsystem* subsystem);

  void Initialize() override;

  bool IsFinished() override;

 private:
  HatchSubsystem* m_hatch;
};

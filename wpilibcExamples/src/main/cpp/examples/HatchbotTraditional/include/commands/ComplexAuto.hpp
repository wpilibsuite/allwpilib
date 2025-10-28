// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/commands2/CommandHelper.hpp>
#include <wpi/commands2/SequentialCommandGroup.hpp>

#include "Constants.hpp"
#include "commands/DriveDistance.hpp"
#include "commands/ReleaseHatch.hpp"

/**
 * A complex auto command that drives forward, releases a hatch, and then drives
 * backward.
 */
class ComplexAuto
    : public wpi::cmd::CommandHelper<wpi::cmd::SequentialCommandGroup,
                                     ComplexAuto> {
 public:
  /**
   * Creates a new ComplexAuto.
   *
   * @param drive The drive subsystem this command will run on
   * @param hatch The hatch subsystem this command will run on
   */
  ComplexAuto(DriveSubsystem* drive, HatchSubsystem* hatch);
};

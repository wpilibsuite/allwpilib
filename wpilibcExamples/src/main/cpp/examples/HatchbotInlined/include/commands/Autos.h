// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command2/CommandPtr.h>

#include "subsystems/DriveSubsystem.h"
#include "subsystems/HatchSubsystem.h"

/** Container for auto command factories. */
namespace autos {

/**
 * A simple auto that drives forward, then stops.
 */
frc::CommandPtr SimpleAuto(DriveSubsystem* drive);

/**
 * A complex auto command that drives forward, releases a hatch, and then drives
 * backward.
 */
frc::CommandPtr ComplexAuto(DriveSubsystem* drive, HatchSubsystem* hatch);

}  // namespace autos

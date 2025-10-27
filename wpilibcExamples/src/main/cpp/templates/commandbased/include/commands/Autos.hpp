// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandPtr.h>

#include "subsystems/ExampleSubsystem.h"

namespace autos {
/**
 * Example static factory for an autonomous command.
 */
frc2::CommandPtr ExampleAuto(ExampleSubsystem* subsystem);
}  // namespace autos

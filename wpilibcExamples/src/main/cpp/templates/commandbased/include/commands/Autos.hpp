// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "subsystems/ExampleSubsystem.hpp"
#include "wpi/commands2/CommandPtr.hpp"

namespace autos {
/**
 * Example static factory for an autonomous command.
 */
wpi::cmd::CommandPtr ExampleAuto(ExampleSubsystem* subsystem);
}  // namespace autos

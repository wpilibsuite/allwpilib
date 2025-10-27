// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/Autos.hpp"

#include <wpi/commands2/Commands.hpp>

#include "commands/ExampleCommand.hpp"

wpi::cmd::CommandPtr autos::ExampleAuto(ExampleSubsystem* subsystem) {
  return wpi::cmd::cmd::Sequence(subsystem->ExampleMethodCommand(),
                             ExampleCommand(subsystem).ToPtr());
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/Autos.h"

#include <frc/command2/Commands.h>

#include "commands/ExampleCommand.h"

frc::CommandPtr autos::ExampleAuto(ExampleSubsystem* subsystem) {
  return frc::cmd::Sequence(subsystem->ExampleMethodCommand(),
                            ExampleCommand(subsystem).ToPtr());
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "frc2/command/CommandHelper.h"
#include "frc2/command/FunctionalCommand.h"
#include "frc2/command/Requirements.h"

namespace frc2 {
/**
 * A command that runs a Runnable continuously.  Has no end condition as-is;
 * either subclass it or use Command.WithTimeout() or
 * Command.Until() to give it one.  If you only wish
 * to execute a Runnable once, use InstantCommand.
 *
 * This class is provided by the NewCommands VendorDep
 */
class RunCommand : public CommandHelper<FunctionalCommand, RunCommand> {
 public:
  /**
   * Creates a new RunCommand.  The Runnable will be run continuously until the
   * command ends.  Does not run when disabled.
   *
   * @param toRun        the Runnable to run
   * @param requirements the subsystems to require
   */
  explicit RunCommand(std::function<void()> toRun,
                      Requirements requirements = {});

  RunCommand(RunCommand&& other) = default;

  RunCommand(const RunCommand& other) = default;
};
}  // namespace frc2

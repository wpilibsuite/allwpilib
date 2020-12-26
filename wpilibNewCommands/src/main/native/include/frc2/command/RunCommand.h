// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>

#include <wpi/ArrayRef.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that runs a Runnable continuously.  Has no end condition as-is;
 * either subclass it or use Command.WithTimeout() or
 * Command.WithInterrupt() to give it one.  If you only wish
 * to execute a Runnable once, use InstantCommand.
 */
class RunCommand : public CommandHelper<CommandBase, RunCommand> {
 public:
  /**
   * Creates a new RunCommand.  The Runnable will be run continuously until the
   * command ends.  Does not run when disabled.
   *
   * @param toRun        the Runnable to run
   * @param requirements the subsystems to require
   */
  RunCommand(std::function<void()> toRun,
             std::initializer_list<Subsystem*> requirements);

  /**
   * Creates a new RunCommand.  The Runnable will be run continuously until the
   * command ends.  Does not run when disabled.
   *
   * @param toRun        the Runnable to run
   * @param requirements the subsystems to require
   */
  RunCommand(std::function<void()> toRun,
             wpi::ArrayRef<Subsystem*> requirements = {});

  RunCommand(RunCommand&& other) = default;

  RunCommand(const RunCommand& other) = default;

  void Execute();

 protected:
  std::function<void()> m_toRun;
};
}  // namespace frc2

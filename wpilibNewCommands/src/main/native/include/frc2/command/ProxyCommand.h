// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <span>

#include <wpi/FunctionExtras.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/SetUtilities.h"

namespace frc2 {
/**
 * Schedules the given command when this command is initialized, and ends when
 * it ends. Useful for forking off from CommandGroups. If this command is
 * interrupted, it will cancel the command.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
class ProxyCommand : public CommandHelper<CommandBase, ProxyCommand> {
 public:
  /**
   * Creates a new ProxyCommand that schedules the supplied command when
   * initialized, and ends when it is no longer scheduled. Useful for lazily
   * creating commands at runtime.
   *
   * @param supplier the command supplier
   */
  explicit ProxyCommand(wpi::unique_function<Command*()> supplier);

  /**
   * Creates a new ProxyCommand that schedules the given command when
   * initialized, and ends when it is no longer scheduled.
   *
   * @param command the command to run by proxy
   */
  explicit ProxyCommand(Command* command);

  /**
   * Creates a new ProxyCommand that schedules the given command when
   * initialized, and ends when it is no longer scheduled.
   *
   * <p>Note that this constructor passes ownership of the given command to the
   * returned ProxyCommand.
   *
   * @param command the command to schedule
   */
  explicit ProxyCommand(std::unique_ptr<Command> command);

  ProxyCommand(ProxyCommand&& other) = default;

  void Initialize() override;

  void End(bool interrupted) override;

  void Execute() override;

  bool IsFinished() override;

 private:
  wpi::unique_function<Command*()> m_supplier;
  Command* m_command = nullptr;
};
}  // namespace frc2

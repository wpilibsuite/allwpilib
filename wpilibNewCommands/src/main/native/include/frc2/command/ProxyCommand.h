// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <wpi/FunctionExtras.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * Schedules a given command when this command is initialized and ends when it
 * ends, but does not directly run it. Use this for including a command in a
 * composition without adding its requirements, <strong>but only if you know
 * what you are doing. If you are unsure, see <a
 * href="https://docs.wpilib.org/en/stable/docs/software/commandbased/command-compositions.html#scheduling-other-commands">the
 * WPILib docs</a> for a complete explanation of proxy semantics.</strong> Do
 * not proxy a command from a subsystem already required by the composition, or
 * else the composition will cancel itself when the proxy is reached. If this
 * command is interrupted, it will cancel the command.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
class ProxyCommand : public CommandHelper<Command, ProxyCommand> {
 public:
  /**
   * Creates a new ProxyCommand that schedules the supplied command when
   * initialized, and ends when it is no longer scheduled. Use this for lazily
   * creating <strong>proxied</strong> commands at runtime. Proxying should only
   * be done to escape from composition requirement semantics, so if only
   * initialization time command construction is needed, use {@link
   * DeferredCommand} instead.
   *
   * @param supplier the command supplier
   * @see DeferredCommand
   */
  explicit ProxyCommand(wpi::unique_function<Command*()> supplier);

  /**
   * Creates a new ProxyCommand that schedules the supplied command when
   * initialized, and ends when it is no longer scheduled. Use this for lazily
   * creating <strong>proxied</strong> commands at runtime. Proxying should only
   * be done to escape from composition requirement semantics, so if only
   * initialization time command construction is needed, use {@link
   * DeferredCommand} instead.
   *
   * @param supplier the command supplier
   * @see DeferredCommand
   */
  explicit ProxyCommand(wpi::unique_function<CommandPtr()> supplier);

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

  bool IsFinished() override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  wpi::unique_function<Command*()> m_supplier;
  Command* m_command = nullptr;
};
}  // namespace frc2

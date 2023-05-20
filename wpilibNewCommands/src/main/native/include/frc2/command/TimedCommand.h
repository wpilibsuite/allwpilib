// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Timer.h>
#include <units/time.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that runs a seconds consumer function continuously with elapsed
 * time since the command has been initialized as the input. Has no end
 * condition as-is; either subclass it or use Command.WithTimeout() or
 * Command.Until() to give it one.  If you only wish
 * to execute a Runnable once, use InstantCommand.
 *
 * This is essentially RunCommand with a second consumer for time.
 *
 * This class is provided by the NewCommands VendorDep
 */
class TimedCommand : public CommandHelper<CommandBase, TimedCommand> {
 public:
  /**
   * Creates a new TimedCommand. The DoubleConsumer will be run continuously
   * with time elapsed since initialization until the command ends.
   *
   * @param toRun the DoubleConsumer to run based on elapsed time
   * @param requirements the subsystems to require
   */
  TimedCommand(std::function<void(units::second_t)> toRun,
               std::initializer_list<Subsystem*> requirements);

  /**
   * Creates a new TimedCommand. The DoubleConsumer will be run continuously
   * with time elapsed since initialization until the command ends.
   *
   * @param toRun the DoubleConsumer to run based on elapsed time
   * @param requirements the subsystems to require
   */
  explicit TimedCommand(std::function<void(units::second_t)> toRun,
                        std::span<Subsystem* const> requirements = {});

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  frc::Timer m_timer;
  std::function<void(units::second_t)> m_output;
};
}  // namespace frc2

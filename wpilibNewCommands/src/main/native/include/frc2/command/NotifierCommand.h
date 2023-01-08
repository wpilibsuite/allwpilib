// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <span>

#include <frc/Notifier.h>
#include <units/time.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that starts a notifier to run the given runnable periodically in a
 * separate thread. Has no end condition as-is; either subclass it or use
 * Command::WithTimeout(double) or Command::Until(BooleanSupplier) to
 * give it one.
 *
 * <p>WARNING: Do not use this class unless you are confident in your ability to
 * make the executed code thread-safe.  If you do not know what "thread-safe"
 * means, that is a good sign that you should not use this class.
 *
 * This class is provided by the NewCommands VendorDep
 */
class NotifierCommand : public CommandHelper<CommandBase, NotifierCommand> {
 public:
  /**
   * Creates a new NotifierCommand.
   *
   * @param toRun        the runnable for the notifier to run
   * @param period       the period at which the notifier should run
   * @param requirements the subsystems required by this command
   */
  NotifierCommand(std::function<void()> toRun, units::second_t period,
                  std::initializer_list<Subsystem*> requirements);

  /**
   * Creates a new NotifierCommand.
   *
   * @param toRun        the runnable for the notifier to run
   * @param period       the period at which the notifier should run
   * @param requirements the subsystems required by this command
   */
  NotifierCommand(std::function<void()> toRun, units::second_t period,
                  std::span<Subsystem* const> requirements = {});

  NotifierCommand(NotifierCommand&& other);

  NotifierCommand(const NotifierCommand& other);

  void Initialize() override;

  void End(bool interrupted) override;

 private:
  std::function<void()> m_toRun;
  frc::Notifier m_notifier;
  units::second_t m_period;
};
}  // namespace frc2

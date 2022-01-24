// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>

#include <wpi/span.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that runs a given runnable when it is initialized, and another
 * runnable when it ends. Useful for running and then stopping a motor, or
 * extending and then retracting a solenoid. Has no end condition as-is; either
 * subclass it or use Command.WithTimeout() or Command.WithInterrupt() to give
 * it one.
 *
 * This class is provided by the NewCommands VendorDep
 */
class StartEndCommand : public CommandHelper<CommandBase, StartEndCommand> {
 public:
  /**
   * Creates a new StartEndCommand.  Will run the given runnables when the
   * command starts and when it ends.
   *
   * @param onInit       the Runnable to run on command init
   * @param onEnd        the Runnable to run on command end
   * @param requirements the subsystems required by this command
   */
  StartEndCommand(std::function<void()> onInit, std::function<void()> onEnd,
                  std::initializer_list<Subsystem*> requirements);

  /**
   * Creates a new StartEndCommand.  Will run the given runnables when the
   * command starts and when it ends.
   *
   * @param onInit       the Runnable to run on command init
   * @param onEnd        the Runnable to run on command end
   * @param requirements the subsystems required by this command
   */
  StartEndCommand(std::function<void()> onInit, std::function<void()> onEnd,
                  wpi::span<Subsystem* const> requirements = {});

  StartEndCommand(StartEndCommand&& other) = default;

  StartEndCommand(const StartEndCommand& other);

  void Initialize() override;

  void End(bool interrupted) override;

 protected:
  std::function<void()> m_onInit;
  std::function<void()> m_onEnd;
};
}  // namespace frc2

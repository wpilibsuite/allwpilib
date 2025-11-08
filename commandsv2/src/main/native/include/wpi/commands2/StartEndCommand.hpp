// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "wpi/commands2/CommandHelper.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"
#include "wpi/commands2/Requirements.hpp"

namespace wpi::cmd {
/**
 * A command that runs a given runnable when it is initialized, and another
 * runnable when it ends. Useful for running and then stopping a motor, or
 * extending and then retracting a solenoid. Has no end condition as-is; either
 * subclass it or use Command.WithTimeout() or Command.Until() to give
 * it one.
 *
 * This class is provided by the NewCommands VendorDep
 */
class StartEndCommand
    : public CommandHelper<FunctionalCommand, StartEndCommand> {
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
                  Requirements requirements = {});

  StartEndCommand(StartEndCommand&& other) = default;

  StartEndCommand(const StartEndCommand& other) = default;
};
}  // namespace wpi::cmd

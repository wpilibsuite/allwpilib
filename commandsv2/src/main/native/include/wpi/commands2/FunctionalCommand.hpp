// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandHelper.hpp"
#include "wpi/commands2/Requirements.hpp"

namespace frc2 {
/**
 * A command that allows the user to pass in functions for each of the basic
 * command methods through the constructor.  Useful for inline definitions of
 * complex commands - note, however, that if a command is beyond a certain
 * complexity it is usually better practice to write a proper class for it than
 * to inline it.
 *
 * This class is provided by the NewCommands VendorDep
 */
class FunctionalCommand : public CommandHelper<Command, FunctionalCommand> {
 public:
  /**
   * Creates a new FunctionalCommand.
   *
   * @param onInit       the function to run on command initialization
   * @param onExecute    the function to run on command execution
   * @param onEnd        the function to run on command end
   * @param isFinished   the function that determines whether the command has
   * finished
   * @param requirements the subsystems required by this command
   */
  FunctionalCommand(std::function<void()> onInit,
                    std::function<void()> onExecute,
                    std::function<void(bool)> onEnd,
                    std::function<bool()> isFinished,
                    Requirements requirements = {});

  FunctionalCommand(FunctionalCommand&& other) = default;

  FunctionalCommand(const FunctionalCommand& other) = default;

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

 private:
  std::function<void()> m_onInit;
  std::function<void()> m_onExecute;
  std::function<void(bool)> m_onEnd;
  std::function<bool()> m_isFinished;
};
}  // namespace frc2

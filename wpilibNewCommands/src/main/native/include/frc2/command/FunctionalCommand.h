/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <initializer_list>

#include <wpi/ArrayRef.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that allows the user to pass in functions for each of the basic
 * command methods through the constructor.  Useful for inline definitions of
 * complex commands - note, however, that if a command is beyond a certain
 * complexity it is usually better practice to write a proper class for it than
 * to inline it.
 */
class FunctionalCommand : public CommandHelper<CommandBase, FunctionalCommand> {
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
                    std::initializer_list<Subsystem*> requirements);

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
                    wpi::ArrayRef<Subsystem*> requirements = {});

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

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <string>

#include <wpi/Twine.h>

#include "frc/commands/Command.h"

namespace frc {

/**
 * A ConditionalCommand is a Command that starts one of two commands.
 *
 * A ConditionalCommand uses the Condition method to determine whether it should
 * run onTrue or onFalse.
 *
 * A ConditionalCommand adds the proper Command to the Scheduler during
 * Initialize() and then IsFinished() will return true once that Command has
 * finished executing.
 *
 * If no Command is specified for onFalse, the occurrence of that condition
 * will be a no-op.
 *
 * A CondtionalCommand will require the superset of subsystems of the onTrue
 * and onFalse commands.
 *
 * @see Command
 * @see Scheduler
 */
class ConditionalCommand : public Command {
 public:
  /**
   * Creates a new ConditionalCommand with given onTrue and onFalse Commands.
   *
   * @param onTrue    The Command to execute if Condition() returns true
   * @param onFalse   The Command to execute if Condition() returns false
   * @param condition A pointer to a parameterless function that returns a bool
   */
  explicit ConditionalCommand(Command* onTrue, Command* onFalse = nullptr,
                              std::function<bool()> condition = nullptr);

  /**
   * Creates a new ConditionalCommand with given onTrue and onFalse Commands.
   *
   * @param name      The name for this command group
   * @param onTrue    The Command to execute if Condition() returns true
   * @param onFalse   The Command to execute if Condition() returns false
   * @param condition A pointer to a parameterless function that returns a bool
   */
  ConditionalCommand(const wpi::Twine& name, Command* onTrue,
                     Command* onFalse = nullptr,
                     std::function<bool()> condition = nullptr);

  virtual ~ConditionalCommand() = default;

  ConditionalCommand(ConditionalCommand&&) = default;
  ConditionalCommand& operator=(ConditionalCommand&&) = default;

 protected:
  /**
   * The Condition to test to determine which Command to run.
   *
   * @return true if m_onTrue should be run or false if m_onFalse should be run.
   */
  virtual bool Condition();

  void _Initialize() override;
  void _Cancel() override;
  bool IsFinished() override;
  void _Interrupted() override;

 private:
  // The Command to execute if Condition() returns true
  Command* m_onTrue;

  // The Command to execute if Condition() returns false
  Command* m_onFalse;

  // Stores command chosen by condition
  Command* m_chosenCommand = nullptr;

  /**
   * The condition to use to determine which Command should be run
   */
  std::function<bool()> m_condition;
};

}  // namespace frc

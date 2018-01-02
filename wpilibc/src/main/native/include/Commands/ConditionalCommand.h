/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <llvm/Twine.h>

#include "Commands/Command.h"
#include "Commands/InstantCommand.h"

namespace frc {

/**
 * A ConditionalCommand is a Command that starts one of two commands.
 *
 * A ConditionalCommand uses m_condition to determine whether it should run
 * m_onTrue or m_onFalse.
 *
 * A ConditionalCommand adds the proper Command to the Scheduler during
 * Initialize() and then IsFinished() will return true once that Command has
 * finished executing.
 *
 * If no Command is specified for m_onFalse, the occurrence of that condition
 * will be a no-op.
 *
 * @see Command
 * @see Scheduler
 */
class ConditionalCommand : public Command {
 public:
  explicit ConditionalCommand(Command* onTrue, Command* onFalse = nullptr);
  ConditionalCommand(const llvm::Twine& name, Command* onTrue,
                     Command* onFalse = nullptr);
  virtual ~ConditionalCommand() = default;

 protected:
  /**
   * The Condition to test to determine which Command to run.
   *
   * @return true if m_onTrue should be run or false if m_onFalse should be run.
   */
  virtual bool Condition() = 0;

  void _Initialize() override;
  void _Cancel() override;
  bool IsFinished() override;
  void Interrupted() override;

 private:
  // The Command to execute if Condition() returns true
  Command* m_onTrue;

  // The Command to execute if Condition() returns false
  Command* m_onFalse;

  // Stores command chosen by condition
  Command* m_chosenCommand = nullptr;
};

}  // namespace frc

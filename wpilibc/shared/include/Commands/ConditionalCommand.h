/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Commands/Command.h"
#include "Commands/Scheduler.h"
/**
 * A {@link ConditionalCommand} is a {@link Command} that starts one of two commands
 *
 * <p>
 * A {@link ConditionalCommand} uses {@link ConditionalCommand#Condition()} to determine whether it should run
 * {@link ConditionalCommand#m_onTrue} or {@link ConditionalCommand#m_onFalse}.
 * </p>
 *
 * <p>
 * A {@link ConditionalCommand} adds the proper {@link Command} to the {@link Scheduler} during {@link ConditionalCommand#Initialize()}
 * and then {@link ConditionalCommand#IsFinished()} will return true.
 * </p>
 * @author Patrick Murphy
 * @author Toby Macaluso
 * @see Command
 * @see Scheduler
 *
 */
class ConditionalCommand : public Command {
 public:
  ConditionalCommand(Command *onTrue, Command *onFalse);
  ConditionalCommand(const std::string &name, Command *onTrue,
                     Command *onFalse);
  virtual ~ConditionalCommand() = default;
 protected:
  virtual bool Condition() = 0;
  virtual void _Initialize();
  virtual void Initialize();
  virtual void Execute();
  virtual void End();
  virtual void Interrupted();
  virtual bool IsFinished();
 private:
  /** The Command to execute if {@link ConditionalCommand#Condition()} returns true*/
  Command *m_onTrue;
  /** The Command to execute if {@link ConditionalCommand#Condition()} returns false*/
  Command *m_onFalse;
  /** Keeps track of if the command has finished*/
  bool m_isFinished = false;
};

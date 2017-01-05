/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/ConditionalCommand.h"
/**
 * Creates a new ConditionalCommand with given onTrue and onFalse Commands
 * @param onTrue The Command to execute if {@link ConditionalCommand#Condition()} returns true
 * @param onFalse The Command to execute if {@link ConditionalCommand#Condition()} returns false
 */
ConditionalCommand::ConditionalCommand(Command *onTrue, Command *onFalse) {
  m_onTrue = onTrue;
  m_onFalse = onFalse;
}
/**
 * Creates a new ConditionalCommand with given onTrue and onFalse Commands
 * @param name the name for this command group
 * @param onTrue The Command to execute if {@link ConditionalCommand#Condition()} returns true
 * @param onFalse The Command to execute if {@link ConditionalCommand#Condition()} returns false
 */
ConditionalCommand::ConditionalCommand(const std::string &name, Command *onTrue,
                                       Command *onFalse)
    : Command(name) {
  m_onTrue = onTrue;
  m_onFalse = onFalse;
}

bool ConditionalCommand::IsFinished() {
  return m_isFinished;
}

void ConditionalCommand::_Initialize() {
  if (Condition()) {
    Scheduler::GetInstance()->AddCommand(m_onTrue);
  } else {
    Scheduler::GetInstance()->AddCommand(m_onFalse);
  }
  m_isFinished = true;
}

void ConditionalCommand::Initialize() {

}

void ConditionalCommand::Execute() {

}

void ConditionalCommand::End() {

}

void ConditionalCommand::Interrupted() {

}

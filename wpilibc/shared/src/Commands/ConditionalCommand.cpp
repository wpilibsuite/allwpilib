/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/ConditionalCommand.h"

#include "Commands/Scheduler.h"

using namespace frc;

/**
 * Creates a new ConditionalCommand with given onTrue and onFalse Commands.
 *
 * @param onTrue The Command to execute if {@link
 * ConditionalCommand#Condition()} returns true
 * @param onFalse The Command to execute if {@link
 * ConditionalCommand#Condition()} returns false
 */
ConditionalCommand::ConditionalCommand(Command* onTrue, Command* onFalse) {
  m_onTrue = onTrue;
  m_onFalse = onFalse;

  for (auto requirement : m_onTrue->GetRequirements()) Requires(requirement);
  for (auto requirement : m_onFalse->GetRequirements()) Requires(requirement);
}

/**
 * Creates a new ConditionalCommand with given onTrue and onFalse Commands.
 *
 * @param name the name for this command group
 * @param onTrue The Command to execute if {@link
 * ConditionalCommand#Condition()} returns true
 * @param onFalse The Command to execute if {@link
 * ConditionalCommand#Condition()} returns false
 */
ConditionalCommand::ConditionalCommand(const std::string& name, Command* onTrue,
                                       Command* onFalse)
    : Command(name) {
  m_onTrue = onTrue;
  m_onFalse = onFalse;

  for (auto requirement : m_onTrue->GetRequirements()) Requires(requirement);
  for (auto requirement : m_onFalse->GetRequirements()) Requires(requirement);
}

void ConditionalCommand::_Initialize() {
  if (Condition()) {
    m_chosenCommand = m_onTrue;
  } else {
    m_chosenCommand = m_onFalse;
  }

  /*
   * This is a hack to make cancelling the chosen command inside a CommandGroup
   * work properly
   */
  m_chosenCommand->ClearRequirements();

  m_chosenCommand->Start();
}

void ConditionalCommand::_Cancel() {
  if (m_chosenCommand != nullptr && m_chosenCommand->IsRunning()) {
    m_chosenCommand->Cancel();
  }

  Command::_Cancel();
}

bool ConditionalCommand::IsFinished() {
  return m_chosenCommand != nullptr && m_chosenCommand->IsRunning() &&
         m_chosenCommand->IsFinished();
}

void ConditionalCommand::Interrupted() {
  if (m_chosenCommand != nullptr && m_chosenCommand->IsRunning()) {
    m_chosenCommand->Cancel();
  }

  Command::Interrupted();
}

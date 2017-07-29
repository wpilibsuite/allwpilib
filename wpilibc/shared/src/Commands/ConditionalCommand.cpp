/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/ConditionalCommand.h"

#include "Commands/Scheduler.h"

static void RequireAll(frc::Command& command, frc::Command* onTrue,
                       frc::Command* onFalse) {
  if (onTrue != nullptr) {
    for (auto requirement : onTrue->GetRequirements())
      command.Requires(requirement);
  }
  if (onFalse != nullptr) {
    for (auto requirement : onFalse->GetRequirements())
      command.Requires(requirement);
  }
}

/**
 * Creates a new ConditionalCommand with given onTrue and onFalse Commands.
 *
 * @param onTrue The Command to execute if {@link
 * ConditionalCommand#Condition()} returns true
 * @param onFalse The Command to execute if {@link
 * ConditionalCommand#Condition()} returns false
 */
frc::ConditionalCommand::ConditionalCommand(Command* onTrue, Command* onFalse) {
  m_onTrue = onTrue;
  m_onFalse = onFalse;

  RequireAll(*this, onTrue, onFalse);
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
frc::ConditionalCommand::ConditionalCommand(const std::string& name,
                                            Command* onTrue, Command* onFalse)
    : Command(name) {
  m_onTrue = onTrue;
  m_onFalse = onFalse;

  RequireAll(*this, onTrue, onFalse);
}

void frc::ConditionalCommand::_Initialize() {
  if (Condition()) {
    m_chosenCommand = m_onTrue;
  } else {
    m_chosenCommand = m_onFalse;
  }

  if (m_chosenCommand != nullptr) {
    /*
     * This is a hack to make cancelling the chosen command inside a
     * CommandGroup work properly
     */
    m_chosenCommand->ClearRequirements();

    m_chosenCommand->Start();
  }
}

void frc::ConditionalCommand::_Cancel() {
  if (m_chosenCommand != nullptr && m_chosenCommand->IsRunning()) {
    m_chosenCommand->Cancel();
  }

  Command::_Cancel();
}

bool frc::ConditionalCommand::IsFinished() {
  return m_chosenCommand != nullptr && m_chosenCommand->IsRunning() &&
         m_chosenCommand->IsFinished();
}

void frc::ConditionalCommand::Interrupted() {
  if (m_chosenCommand != nullptr && m_chosenCommand->IsRunning()) {
    m_chosenCommand->Cancel();
  }

  Command::Interrupted();
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/ConditionalCommand.h"

#include <wpi/raw_ostream.h>

#include "frc/commands/Scheduler.h"

using namespace frc;

static void RequireAll(Command& command, Command* onTrue, Command* onFalse) {
  if (onTrue != nullptr) {
    for (auto requirement : onTrue->GetRequirements())
      command.Requires(requirement);
  }
  if (onFalse != nullptr) {
    for (auto requirement : onFalse->GetRequirements())
      command.Requires(requirement);
  }
}

ConditionalCommand::ConditionalCommand(Command* onTrue, Command* onFalse,
                                       std::function<bool()> condition) {
  m_onTrue = onTrue;
  m_onFalse = onFalse;
  m_condition = condition;

  RequireAll(*this, onTrue, onFalse);
}

ConditionalCommand::ConditionalCommand(const wpi::Twine& name, Command* onTrue,
                                       Command* onFalse,
                                       std::function<bool()> condition)
    : Command(name) {
  m_onTrue = onTrue;
  m_onFalse = onFalse;
  m_condition = condition;

  RequireAll(*this, onTrue, onFalse);
}

bool ConditionalCommand::Condition() {
  if (m_condition == nullptr) {
    wpi::errs() << "Error: either override Condition() or pass a predicate to "
                   "the constructor\n";
    return true;
  } else {
    return m_condition();
  }
}

void ConditionalCommand::_Initialize() {
  if (Condition()) {
    m_chosenCommand = m_onTrue;
  } else {
    m_chosenCommand = m_onFalse;
  }

  if (m_chosenCommand != nullptr) {
    // This is a hack to make cancelling the chosen command inside a
    // CommandGroup work properly
    m_chosenCommand->ClearRequirements();

    m_chosenCommand->Start();
  }
  Command::_Initialize();
}

void ConditionalCommand::_Cancel() {
  if (m_chosenCommand != nullptr && m_chosenCommand->IsRunning()) {
    m_chosenCommand->Cancel();
  }

  Command::_Cancel();
}

bool ConditionalCommand::IsFinished() {
  if (m_chosenCommand != nullptr) {
    return m_chosenCommand->IsCompleted();
  } else {
    return true;
  }
}

void ConditionalCommand::_Interrupted() {
  if (m_chosenCommand != nullptr && m_chosenCommand->IsRunning()) {
    m_chosenCommand->Cancel();
  }

  Command::_Interrupted();
}

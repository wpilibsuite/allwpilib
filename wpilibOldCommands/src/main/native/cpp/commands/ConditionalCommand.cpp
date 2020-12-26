// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/ConditionalCommand.h"

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

ConditionalCommand::ConditionalCommand(Command* onTrue, Command* onFalse) {
  m_onTrue = onTrue;
  m_onFalse = onFalse;

  RequireAll(*this, onTrue, onFalse);
}

ConditionalCommand::ConditionalCommand(const wpi::Twine& name, Command* onTrue,
                                       Command* onFalse)
    : Command(name) {
  m_onTrue = onTrue;
  m_onFalse = onFalse;

  RequireAll(*this, onTrue, onFalse);
}

void ConditionalCommand::_Initialize() {
  if (Condition()) {
    m_chosenCommand = m_onTrue;
  } else {
    m_chosenCommand = m_onFalse;
  }

  if (m_chosenCommand != nullptr) {
    // This is a hack to make canceling the chosen command inside a
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

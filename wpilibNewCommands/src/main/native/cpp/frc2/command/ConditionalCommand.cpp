// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/ConditionalCommand.h"

using namespace frc2;

ConditionalCommand::ConditionalCommand(std::unique_ptr<Command>&& onTrue,
                                       std::unique_ptr<Command>&& onFalse,
                                       std::function<bool()> condition)
    : m_condition{std::move(condition)} {
  CommandScheduler::GetInstance().RequireUngrouped(
      {onTrue.get(), onFalse.get()});

  m_onTrue = std::move(onTrue);
  m_onFalse = std::move(onFalse);

  m_onTrue->SetComposed(true);
  m_onFalse->SetComposed(true);

  m_runsWhenDisabled &= m_onTrue->RunsWhenDisabled();
  m_runsWhenDisabled &= m_onFalse->RunsWhenDisabled();

  AddRequirements(m_onTrue->GetRequirements());
  AddRequirements(m_onFalse->GetRequirements());
}

void ConditionalCommand::Initialize() {
  if (m_condition()) {
    m_selectedCommand = m_onTrue.get();
  } else {
    m_selectedCommand = m_onFalse.get();
  }
  m_selectedCommand->Initialize();
}

void ConditionalCommand::Execute() {
  m_selectedCommand->Execute();
}

void ConditionalCommand::End(bool interrupted) {
  m_selectedCommand->End(interrupted);
}

bool ConditionalCommand::IsFinished() {
  return m_selectedCommand->IsFinished();
}

bool ConditionalCommand::RunsWhenDisabled() const {
  return m_runsWhenDisabled;
}

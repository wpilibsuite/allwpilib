/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/ConditionalCommand.h"

using namespace frc2;

ConditionalCommand::ConditionalCommand(std::unique_ptr<Command>&& onTrue,
                                       std::unique_ptr<Command>&& onFalse,
                                       std::function<bool()> condition)
    : m_condition{std::move(condition)} {
  if (!CommandGroupBase::RequireUngrouped({onTrue.get(), onFalse.get()})) {
    return;
  }

  m_onTrue = std::move(onTrue);
  m_onFalse = std::move(onFalse);

  m_onTrue->SetGrouped(true);
  m_onFalse->SetGrouped(true);

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

void ConditionalCommand::Execute() { m_selectedCommand->Execute(); }

void ConditionalCommand::End(bool interrupted) {
  m_selectedCommand->End(interrupted);
}

bool ConditionalCommand::IsFinished() {
  return m_selectedCommand->IsFinished();
}

bool ConditionalCommand::RunsWhenDisabled() const { return m_runsWhenDisabled; }

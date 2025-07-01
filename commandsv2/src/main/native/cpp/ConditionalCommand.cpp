// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/ConditionalCommand.hpp"

#include <memory>
#include <string>
#include <utility>

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi::cmd;

ConditionalCommand::ConditionalCommand(std::unique_ptr<Command>&& onTrue,
                                       std::unique_ptr<Command>&& onFalse,
                                       std::function<bool()> condition)
    : m_condition{std::move(condition)} {
  CommandScheduler::GetInstance().RequireUngroupedAndUnscheduled(
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

Command::InterruptionBehavior ConditionalCommand::GetInterruptionBehavior()
    const {
  if (m_onTrue->GetInterruptionBehavior() ==
          InterruptionBehavior::kCancelSelf ||
      m_onFalse->GetInterruptionBehavior() ==
          InterruptionBehavior::kCancelSelf) {
    return InterruptionBehavior::kCancelSelf;
  } else {
    return InterruptionBehavior::kCancelIncoming;
  }
}

void ConditionalCommand::LogTo(wpi::TelemetryTable& table) const {
  Command::LogTo(table);
  table.Log("onTrue", m_onTrue->GetName());
  table.Log("onFalse", m_onFalse->GetName());
  table.Log("selected", m_selectedCommand ? m_selectedCommand->GetName()
                                          : std::string{"null"});
}

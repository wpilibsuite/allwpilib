// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/DeferredCommand.h"

#include <utility>

#include <wpi/sendable/SendableBuilder.h>

#include "frc2/command/Commands.h"

using namespace frc2;

DeferredCommand::DeferredCommand(wpi::unique_function<CommandPtr()> supplier,
                                 Requirements requirements)
    : m_supplier{std::move(supplier)} {
  AddRequirements(requirements);
}

void DeferredCommand::Initialize() {
  m_command = m_supplier().Unwrap();
  CommandScheduler::GetInstance().RequireUngrouped(m_command.get());
  m_command->SetComposed(true);
  m_command->Initialize();
}

void DeferredCommand::Execute() {
  m_command->Execute();
}

void DeferredCommand::End(bool interrupted) {
  m_command->End(interrupted);
  m_command =
      cmd::Print("[DeferredCommand] Lifecycle function called out-of-order!")
          .WithName("none")
          .Unwrap();
}

bool DeferredCommand::IsFinished() {
  return m_command->IsFinished();
}

void DeferredCommand::InitSendable(wpi::SendableBuilder& builder) {
  Command::InitSendable(builder);
  builder.AddStringProperty(
      "deferred", [this] { return m_command->GetName(); }, nullptr);
}

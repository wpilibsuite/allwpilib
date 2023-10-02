// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/DeferredCommand.h"

#include <wpi/sendable/SendableBuilder.h>

#include "frc2/command/PrintCommand.h"

using namespace frc2;

DeferredCommand::DeferredCommand(wpi::unique_function<Command*()> supplier,
                                 Requirements requirements)
    : m_supplier{std::move(supplier)} {
  AddRequirements(requirements);
}

DeferredCommand::DeferredCommand(wpi::unique_function<CommandPtr()> supplier,
                                 Requirements requirements)
    : DeferredCommand(
          [lambdaSupplier = std::move(supplier),
           holder = std::optional<CommandPtr>{}]() mutable {
            holder = lambdaSupplier();
            return holder->get();
          },
          requirements) {}

void DeferredCommand::Initialize() {
  auto cmd = m_supplier();
  if (cmd != nullptr) {
    m_command = cmd;
    CommandScheduler::GetInstance().RequireUngrouped(m_command);
    m_command->SetComposed(true);
  }
  m_command->Initialize();
}

void DeferredCommand::Execute() {
  m_command->Execute();
}

void DeferredCommand::End(bool interrupted) {
  m_command->End(interrupted);
  m_command = &m_nullCommand;
}

bool DeferredCommand::IsFinished() {
  return m_command->IsFinished();
}

void DeferredCommand::InitSendable(wpi::SendableBuilder& builder) {
  Command::InitSendable(builder);
  builder.AddStringProperty(
      "deferred",
      [this] {
        if (m_command != &m_nullCommand) {
          return m_command->GetName();
        } else {
          return std::string{"null"};
        }
      },
      nullptr);
}

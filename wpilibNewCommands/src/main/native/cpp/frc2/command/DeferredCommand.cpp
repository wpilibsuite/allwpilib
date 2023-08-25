// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/DeferredCommand.h"

#include <wpi/sendable/SendableBuilder.h>

#include "frc2/command/PrintCommand.h"

using namespace frc2;

DeferredCommand::DeferredCommand(wpi::unique_function<Command*()> supplier,
                                 std::span<Subsystem* const> requirements)
    : m_supplier{std::move(supplier)} {
  AddRequirements(requirements);
  ResetInternalCommand();
}

DeferredCommand::DeferredCommand(wpi::unique_function<Command*()> supplier,
                                 std::initializer_list<Subsystem*> requirements)
    : m_supplier{std::move(supplier)} {
  AddRequirements(requirements);
  ResetInternalCommand();
}

void DeferredCommand::Initialize() {
  auto cmd = m_supplier();
  if (cmd != nullptr) {
    m_command = cmd;
  }
  CommandScheduler::GetInstance().RequireUngrouped(m_command);
  m_command->SetComposed(true);
  m_command->Initialize();
}

void DeferredCommand::Execute() {
  m_command->Execute();
}

void DeferredCommand::End(bool interrupted) {
  m_command->End(interrupted);
}

bool DeferredCommand::IsFinished() {
  return m_command->IsFinished();
}

void DeferredCommand::InitSendable(wpi::SendableBuilder& builder) {
  Command::InitSendable(builder);
  builder.AddStringProperty(
      "deferred",
      [this] {
        if (m_command) {
          return m_command->GetName();
        } else {
          return std::string{"null"};
        }
      },
      nullptr);
}

void DeferredCommand::ResetInternalCommand() {
  m_command = new PrintCommand("[DeferredCommand] Supplied command was null!");
}

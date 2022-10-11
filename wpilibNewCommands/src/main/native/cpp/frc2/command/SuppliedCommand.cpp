// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/SuppliedCommand.h"

using namespace frc2;

SuppliedCommand::SuppliedCommand(std::function<Command*()> supplier,
                                 std::initializer_list<Subsystem*> requirements)
    : m_supplier{std::move(supplier)} {
  AddRequirements(requirements);
}

void SuppliedCommand::Initialize() {
  m_selectedCommand = m_supplier();
}

void SuppliedCommand::Execute() {
  m_selectedCommand->Execute();
}

void SuppliedCommand::End(bool interrupted) {
  m_selectedCommand->End(interrupted);
}

bool SuppliedCommand::IsFinished() {
  return m_selectedCommand->IsFinished();
}

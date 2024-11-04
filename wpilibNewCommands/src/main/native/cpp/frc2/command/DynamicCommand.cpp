// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/DynamicCommand.h"

#include <string>
#include <utility>

frc2::DynamicCommand::DynamicCommand(wpi::unique_function<Command*()> supplier)
    : m_supplier(std::move(supplier)) {}

frc2::DynamicCommand::DynamicCommand(
    wpi::unique_function<CommandPtr()> supplier)
    : DynamicCommand([supplier = std::move(supplier),
                      holder = std::optional<CommandPtr>{}]() mutable {
        holder = supplier();
        return holder->get();
      }) {}

void frc2::DynamicCommand::Initialize() {
  m_command = m_supplier();
  m_command->Schedule();
}

void frc2::DynamicCommand::End(bool interrupted) {
  if (interrupted) {
    m_command->Cancel();
  }
  m_command = nullptr;
}

bool frc2::DynamicCommand::IsFinished() {
  // because we're between `initialize` and `end`, `m_command` is necessarily
  // not null but if called otherwise and m_command is null, it's UB, so we can
  // do whatever we want -- like return true.
  return m_command == nullptr || !m_command->IsScheduled();
}

void frc2::DynamicCommand::InitSendable(wpi::SendableBuilder& builder) {
  Command::InitSendable(builder);
  builder.AddStringProperty(
      "dynamic",
      [this] {
        if (m_command) {
          return m_command->GetName();
        } else {
          return std::string{"null"};
        }
      },
      nullptr);
}

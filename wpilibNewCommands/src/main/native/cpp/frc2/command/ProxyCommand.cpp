// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/ProxyCommand.h"

using namespace frc2;

ProxyCommand::ProxyCommand(wpi::unique_function<Command*()> supplier)
    : m_supplier(std::move(supplier)) {}

ProxyCommand::ProxyCommand(Command* command)
    : m_supplier([command] { return command; }) {}

ProxyCommand::ProxyCommand(std::unique_ptr<Command> command)
    : m_supplier([command = std::move(command)] { return command.get(); }) {}

void ProxyCommand::Initialize() {
  m_command = m_supplier();
  m_command->Schedule();
}

void ProxyCommand::End(bool interrupted) {
  if (interrupted) {
    m_command->Cancel();
  }
  m_command = nullptr;
}

void ProxyCommand::Execute() {}

bool ProxyCommand::IsFinished() {
  // because we're between `initialize` and `end`, `m_command` is necessarily
  // not null but if called otherwise and m_command is null, it's UB, so we can
  // do whatever we want -- like return true.
  return m_command == nullptr || !m_command->IsScheduled();
}

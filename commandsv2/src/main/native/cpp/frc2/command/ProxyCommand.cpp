// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/ProxyCommand.hpp"

#include <string>
#include <utility>

#include <fmt/format.h>

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi::cmd;

ProxyCommand::ProxyCommand(Command* command)
    : m_supplier([command] { return command; }) {
  SetName(fmt::format("Proxy({})", command->GetName()));
}

ProxyCommand::ProxyCommand(std::unique_ptr<Command> command) {
  SetName(fmt::format("Proxy({})", command->GetName()));
  m_supplier = [command = std::move(command)] { return command.get(); };
}

void ProxyCommand::Initialize() {
  m_command = m_supplier();
  wpi::cmd::CommandScheduler::GetInstance().Schedule(m_command);
}

void ProxyCommand::End(bool interrupted) {
  if (interrupted) {
    m_command->Cancel();
  }
  m_command = nullptr;
}

bool ProxyCommand::IsFinished() {
  // because we're between `initialize` and `end`, `m_command` is necessarily
  // not null but if called otherwise and m_command is null, it's UB, so we can
  // do whatever we want -- like return true.
  return m_command == nullptr || !m_command->IsScheduled();
}

void ProxyCommand::LogTo(wpi::TelemetryTable& table) const {
  Command::LogTo(table);
  table.Log("proxied", m_command ? m_command->GetName() : std::string{"null"});
}

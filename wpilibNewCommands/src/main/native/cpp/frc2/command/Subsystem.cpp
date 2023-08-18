// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/Subsystem.h"

#include "frc2/command/CommandPtr.h"
#include "frc2/command/Commands.h"

using namespace frc2;

Subsystem::Subsystem() : Subsystem::Subsystem(true) {}

Subsystem::Subsystem(bool enabled) {
  wpi::SendableRegistry::AddLW(this, GetTypeName(*this));
  m_enabled = enabled;
  m_defaultCommand = nullptr;
  if (enabled) {
    CommandScheduler::GetInstance().RegisterSubsystem({this});
  }
}

Subsystem::~Subsystem() {
  if (m_enabled) {
    CommandScheduler::GetInstance().UnregisterSubsystem(this);
  }
}

void Subsystem::Periodic() {}

void Subsystem::SimulationPeriodic() {}

void Subsystem::Enable() {
  m_enabled = true;
  Register();
  if (m_defaultCommand != nullptr) {
    SetDefaultCommand(std::move(*m_defaultCommand));
  }
}

void Subsystem::Disable() {
  m_enabled = false;
  CommandScheduler::GetInstance().UnregisterSubsystem(this);
}

void Subsystem::SetDefaultCommand(CommandPtr&& defaultCommand) {
  *m_defaultCommand = std::move(defaultCommand);
  if (m_enabled) {
    CommandScheduler::GetInstance().SetDefaultCommand(this,
                                                    std::move(defaultCommand));
  }
}

void Subsystem::RemoveDefaultCommand() {
  if (m_enabled) {
		CommandScheduler::GetInstance().RemoveDefaultCommand(this);
	}
  m_defaultCommand = nullptr;
}

Command* Subsystem::GetDefaultCommand() const {
  return m_defaultCommand->get();
}

Command* Subsystem::GetCurrentCommand() const {
  return m_enabled ? CommandScheduler::GetInstance().Requiring(this) : nullptr;
}

std::string Subsystem::GetName() const {
  return wpi::SendableRegistry::GetName(this);
}

void Subsystem::SetName(std::string_view name) {
  wpi::SendableRegistry::SetName(this, name);
}

std::string Subsystem::GetSubsystem() const {
  return wpi::SendableRegistry::GetSubsystem(this);
}

void Subsystem::SetSubsystem(std::string_view name) {
  wpi::SendableRegistry::SetSubsystem(this, name);
}

void Subsystem::AddChild(std::string name, wpi::Sendable* child) {
  wpi::SendableRegistry::AddLW(child, GetSubsystem(), name);
}

void Subsystem::Register() {
  return CommandScheduler::GetInstance().RegisterSubsystem(this);
}

CommandPtr Subsystem::RunOnce(std::function<void()> action) {
  return cmd::RunOnce(std::move(action), {this});
}

CommandPtr Subsystem::Run(std::function<void()> action) {
  return cmd::Run(std::move(action), {this});
}

CommandPtr Subsystem::StartEnd(std::function<void()> start,
                               std::function<void()> end) {
  return cmd::StartEnd(std::move(start), std::move(end), {this});
}

CommandPtr Subsystem::RunEnd(std::function<void()> run,
                             std::function<void()> end) {
  return cmd::RunEnd(std::move(run), std::move(end), {this});
}

void Subsystem::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Subsystem");
  builder.AddBooleanProperty(
      ".hasDefault", [this] { return GetDefaultCommand() != nullptr; },
      nullptr);
  builder.AddStringProperty(
      ".default",
      [this]() -> std::string {
        auto command = GetDefaultCommand();
        if (command == nullptr) {
          return "none";
        }
        return command->GetName();
      },
      nullptr);
  builder.AddBooleanProperty(
      ".hasCommand", [this] { return GetCurrentCommand() != nullptr; },
      nullptr);
  builder.AddStringProperty(
      ".command",
      [this]() -> std::string {
        auto command = GetCurrentCommand();
        if (command == nullptr) {
          return "none";
        }
        return command->GetName();
      },
      nullptr);
}

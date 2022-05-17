// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/Subsystem.h"

#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandScheduler.h"

using namespace frc2;

Subsystem::Subsystem() {
  wpi::SendableRegistry::AddLW(this, GetTypeName(*this));
  CommandScheduler::GetInstance().RegisterSubsystem({this});
}

void Subsystem::Periodic() {}

void Subsystem::SimulationPeriodic() {}

Command* Subsystem::GetDefaultCommand() const {
  return CommandScheduler::GetInstance().GetDefaultCommand(this);
}

Command* Subsystem::GetCurrentCommand() const {
  return CommandScheduler::GetInstance().Requiring(this);
}

void Subsystem::Register() {
  return CommandScheduler::GetInstance().RegisterSubsystem(this);
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

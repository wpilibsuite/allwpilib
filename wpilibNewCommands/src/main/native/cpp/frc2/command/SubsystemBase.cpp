// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/SubsystemBase.h"

#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandScheduler.h"

using namespace frc2;

SubsystemBase::SubsystemBase() {
  wpi::SendableRegistry::GetInstance().AddLW(this, GetTypeName(*this));
  CommandScheduler::GetInstance().RegisterSubsystem({this});
}

void SubsystemBase::InitSendable(wpi::SendableBuilder& builder) {
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

std::string SubsystemBase::GetName() const {
  return wpi::SendableRegistry::GetInstance().GetName(this);
}

void SubsystemBase::SetName(std::string_view name) {
  wpi::SendableRegistry::GetInstance().SetName(this, name);
}

std::string SubsystemBase::GetSubsystem() const {
  return wpi::SendableRegistry::GetInstance().GetSubsystem(this);
}

void SubsystemBase::SetSubsystem(std::string_view name) {
  wpi::SendableRegistry::GetInstance().SetSubsystem(this, name);
}

void SubsystemBase::AddChild(std::string name, wpi::Sendable* child) {
  auto& registry = wpi::SendableRegistry::GetInstance();
  registry.AddLW(child, GetSubsystem(), name);
}

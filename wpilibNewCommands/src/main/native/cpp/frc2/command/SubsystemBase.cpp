// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/SubsystemBase.h"

#include <frc/smartdashboard/SendableBuilder.h>
#include <frc/smartdashboard/SendableRegistry.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandScheduler.h"

using namespace frc2;

SubsystemBase::SubsystemBase() {
  frc::SendableRegistry::GetInstance().AddLW(this, GetTypeName(*this));
  CommandScheduler::GetInstance().RegisterSubsystem({this});
}

void SubsystemBase::InitSendable(frc::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Subsystem");
  builder.AddBooleanProperty(
      ".hasDefault", [this] { return GetDefaultCommand() != nullptr; },
      nullptr);
  builder.AddStringProperty(
      ".default",
      [this]() -> std::string {
        auto command = GetDefaultCommand();
        if (command == nullptr)
          return "none";
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
        if (command == nullptr)
          return "none";
        return command->GetName();
      },
      nullptr);
}

std::string SubsystemBase::GetName() const {
  return frc::SendableRegistry::GetInstance().GetName(this);
}

void SubsystemBase::SetName(const wpi::Twine& name) {
  frc::SendableRegistry::GetInstance().SetName(this, name);
}

std::string SubsystemBase::GetSubsystem() const {
  return frc::SendableRegistry::GetInstance().GetSubsystem(this);
}

void SubsystemBase::SetSubsystem(const wpi::Twine& name) {
  frc::SendableRegistry::GetInstance().SetSubsystem(this, name);
}

void SubsystemBase::AddChild(std::string name, frc::Sendable* child) {
  auto& registry = frc::SendableRegistry::GetInstance();
  registry.AddLW(child, GetSubsystem(), name);
}

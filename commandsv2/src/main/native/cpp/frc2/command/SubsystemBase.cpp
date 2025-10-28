// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/SubsystemBase.hpp"

#include <string>

#include <wpi/util/sendable/SendableBuilder.hpp>
#include <wpi/util/sendable/SendableRegistry.hpp>

#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandScheduler.hpp"

using namespace wpi::cmd;

SubsystemBase::SubsystemBase() {
  wpi::util::SendableRegistry::Add(this, GetTypeName(*this));
  CommandScheduler::GetInstance().RegisterSubsystem({this});
}

SubsystemBase::SubsystemBase(std::string_view name) {
  wpi::util::SendableRegistry::Add(this, name);
  CommandScheduler::GetInstance().RegisterSubsystem({this});
}

void SubsystemBase::InitSendable(wpi::util::SendableBuilder& builder) {
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
  return wpi::util::SendableRegistry::GetName(this);
}

void SubsystemBase::SetName(std::string_view name) {
  wpi::util::SendableRegistry::SetName(this, name);
}

std::string SubsystemBase::GetSubsystem() const {
  return wpi::util::SendableRegistry::GetSubsystem(this);
}

void SubsystemBase::SetSubsystem(std::string_view name) {
  wpi::util::SendableRegistry::SetSubsystem(this, name);
}

void SubsystemBase::AddChild(std::string name, wpi::util::Sendable* child) {
  wpi::util::SendableRegistry::Add(child, GetSubsystem(), name);
}

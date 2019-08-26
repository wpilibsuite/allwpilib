/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/SubsystemBase.h"

#include <frc/livewindow/LiveWindow.h>
#include <frc/smartdashboard/SendableBuilder.h>
#include <frc2/command/Command.h>
#include <frc2/command/CommandScheduler.h>

using namespace frc2;

SubsystemBase::SubsystemBase() {
  m_name = GetTypeName(*this);
  CommandScheduler::GetInstance().RegisterSubsystem({this});
}

void SubsystemBase::InitSendable(frc::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Subsystem");
  builder.AddBooleanProperty(".hasDefault",
                             [this] { return GetDefaultCommand() != nullptr; },
                             nullptr);
  builder.AddStringProperty(".default",
                            [this]() -> std::string {
                              auto command = GetDefaultCommand();
                              if (command == nullptr) return "none";
                              return command->GetName();
                            },
                            nullptr);
  builder.AddBooleanProperty(".hasCommand",
                             [this] { return GetCurrentCommand() != nullptr; },
                             nullptr);
  builder.AddStringProperty(".command",
                            [this]() -> std::string {
                              auto command = GetCurrentCommand();
                              if (command == nullptr) return "none";
                              return command->GetName();
                            },
                            nullptr);
}

std::string SubsystemBase::GetName() const { return m_name; }

void SubsystemBase::SetName(const wpi::Twine& name) { m_name = name.str(); }

std::string SubsystemBase::GetSubsystem() const { return GetName(); }

void SubsystemBase::SetSubsystem(const wpi::Twine& name) { SetName(name); }

void SubsystemBase::AddChild(std::string name, frc::Sendable* child) {
  child->SetName(name);
  frc::LiveWindow::GetInstance()->Add(child);
}

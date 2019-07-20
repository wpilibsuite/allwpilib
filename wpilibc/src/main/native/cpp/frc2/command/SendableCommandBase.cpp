/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/SendableCommandBase.h"

#include <frc/smartdashboard/SendableBuilder.h>
#include <frc2/command/CommandScheduler.h>
#include <frc2/command/SetUtilities.h>

using namespace frc2;

SendableCommandBase::SendableCommandBase() {
  m_name = Command::GetName();
  m_subsystem = "Unknown";
}

SendableCommandBase::SendableCommandBase(const SendableCommandBase& other)
    : Sendable{}, Command{} {
  m_name = other.m_name;
  m_subsystem = other.m_subsystem;
  m_requirements = other.m_requirements;
}

void SendableCommandBase::AddRequirements(
    std::initializer_list<Subsystem*> requirements) {
  m_requirements.insert(requirements.begin(), requirements.end());
}

void SendableCommandBase::AddRequirements(
    wpi::SmallSet<Subsystem*, 4> requirements) {
  m_requirements.insert(requirements.begin(), requirements.end());
}

wpi::SmallSet<Subsystem*, 4> SendableCommandBase::GetRequirements() const {
  return m_requirements;
}

void SendableCommandBase::SetName(const wpi::Twine& name) {
  m_name = name.str();
}

std::string SendableCommandBase::GetName() const { return m_name; }

std::string SendableCommandBase::GetSubsystem() const { return m_subsystem; }

void SendableCommandBase::SetSubsystem(const wpi::Twine& subsystem) {
  m_subsystem = subsystem.str();
}

void SendableCommandBase::InitSendable(frc::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Command");
  builder.AddStringProperty(".name", [this] { return GetName(); }, nullptr);
  builder.AddBooleanProperty("running", [this] { return IsScheduled(); },
                             [this](bool value) {
                               bool isScheduled = IsScheduled();
                               if (value && !isScheduled) {
                                 Schedule();
                               } else if (!value && isScheduled) {
                                 Cancel();
                               }
                             });
}

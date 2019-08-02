/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/smartdashboard/SendableBase.h"

#include <utility>

#include "frc/livewindow/LiveWindow.h"

using namespace frc;

SendableBase::SendableBase(bool addLiveWindow) {
  if (addLiveWindow) LiveWindow::GetInstance()->Add(this);
}

SendableBase::~SendableBase() { LiveWindow::GetInstance()->Remove(this); }

std::string SendableBase::GetName() const {
  return m_name;
}

void SendableBase::SetName(const wpi::Twine& name) {
  m_name = name.str();
}

std::string SendableBase::GetSubsystem() const {
  return m_subsystem;
}

void SendableBase::SetSubsystem(const wpi::Twine& subsystem) {
  m_subsystem = subsystem.str();
}

void SendableBase::AddChild(std::shared_ptr<Sendable> child) {
  LiveWindow::GetInstance()->AddChild(this, child);
}

void SendableBase::AddChild(void* child) {
  LiveWindow::GetInstance()->AddChild(this, child);
}

void SendableBase::SetName(const wpi::Twine& moduleType, int channel) {
  SetName(moduleType + wpi::Twine('[') + wpi::Twine(channel) + wpi::Twine(']'));
}

void SendableBase::SetName(const wpi::Twine& moduleType, int moduleNumber,
                           int channel) {
  SetName(moduleType + wpi::Twine('[') + wpi::Twine(moduleNumber) +
          wpi::Twine(',') + wpi::Twine(channel) + wpi::Twine(']'));
}

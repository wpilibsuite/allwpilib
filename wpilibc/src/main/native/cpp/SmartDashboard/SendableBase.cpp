/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SendableBase.h"

#include "LiveWindow/LiveWindow.h"

using namespace frc;

/**
 * Creates an instance of the sensor base.
 *
 * @param addLiveWindow if true, add this Sendable to LiveWindow
 */
SendableBase::SendableBase(bool addLiveWindow) {
  if (addLiveWindow) LiveWindow::GetInstance()->Add(this);
}

/**
 * Free the resources used by this object.
 */
SendableBase::~SendableBase() { LiveWindow::GetInstance()->Remove(this); }

std::string SendableBase::GetName() const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  return m_name;
}

void SendableBase::SetName(const llvm::Twine& name) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  m_name = name.str();
}

std::string SendableBase::GetSubsystem() const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  return m_subsystem;
}

void SendableBase::SetSubsystem(const llvm::Twine& subsystem) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  m_subsystem = subsystem.str();
}

/**
 * Add a child component.
 *
 * @param child child component
 */
void SendableBase::AddChild(std::shared_ptr<Sendable> child) {
  LiveWindow::GetInstance()->AddChild(this, child);
}

/**
 * Add a child component.
 *
 * @param child child component
 */
void SendableBase::AddChild(void* child) {
  LiveWindow::GetInstance()->AddChild(this, child);
}

/**
 * Sets the name of the sensor with a channel number.
 *
 * @param moduleType A string that defines the module name in the label for the
 *                   value
 * @param channel    The channel number the device is plugged into
 */
void SendableBase::SetName(const llvm::Twine& moduleType, int channel) {
  SetName(moduleType + llvm::Twine('[') + llvm::Twine(channel) +
          llvm::Twine(']'));
}

/**
 * Sets the name of the sensor with a module and channel number.
 *
 * @param moduleType   A string that defines the module name in the label for
 *                     the value
 * @param moduleNumber The number of the particular module type
 * @param channel      The channel number the device is plugged into (usually
 * PWM)
 */
void SendableBase::SetName(const llvm::Twine& moduleType, int moduleNumber,
                           int channel) {
  SetName(moduleType + llvm::Twine('[') + llvm::Twine(moduleNumber) +
          llvm::Twine(',') + llvm::Twine(channel) + llvm::Twine(']'));
}

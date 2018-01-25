/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <llvm/Twine.h>
#include <support/deprecated.h>

#include "SmartDashboard/Sendable.h"

namespace frc {

/**
 * The LiveWindow class is the public interface for putting sensors and
 * actuators on the LiveWindow.
 */
class LiveWindow {
 public:
  LiveWindow(const LiveWindow&) = delete;
  LiveWindow& operator=(const LiveWindow&) = delete;

  static LiveWindow* GetInstance();

  WPI_DEPRECATED("no longer required")
  void Run() { UpdateValues(); }

  WPI_DEPRECATED("use Sendable::SetName() instead")
  void AddSensor(const llvm::Twine& subsystem, const llvm::Twine& name,
                 Sendable* component);
  WPI_DEPRECATED("use Sendable::SetName() instead")
  void AddSensor(const llvm::Twine& subsystem, const llvm::Twine& name,
                 Sendable& component);
  WPI_DEPRECATED("use Sendable::SetName() instead")
  void AddSensor(const llvm::Twine& subsystem, const llvm::Twine& name,
                 std::shared_ptr<Sendable> component);
  WPI_DEPRECATED("use Sendable::SetName() instead")
  void AddActuator(const llvm::Twine& subsystem, const llvm::Twine& name,
                   Sendable* component);
  WPI_DEPRECATED("use Sendable::SetName() instead")
  void AddActuator(const llvm::Twine& subsystem, const llvm::Twine& name,
                   Sendable& component);
  WPI_DEPRECATED("use Sendable::SetName() instead")
  void AddActuator(const llvm::Twine& subsystem, const llvm::Twine& name,
                   std::shared_ptr<Sendable> component);

  WPI_DEPRECATED("use SensorBase::SetName() instead")
  void AddSensor(const llvm::Twine& type, int channel, Sendable* component);
  WPI_DEPRECATED("use SensorBase::SetName() instead")
  void AddActuator(const llvm::Twine& type, int channel, Sendable* component);
  WPI_DEPRECATED("use SensorBase::SetName() instead")
  void AddActuator(const llvm::Twine& type, int module, int channel,
                   Sendable* component);

  void Add(std::shared_ptr<Sendable> component);
  void Add(Sendable* component);
  void AddChild(Sendable* parent, std::shared_ptr<Sendable> component);
  void AddChild(Sendable* parent, void* component);
  void Remove(Sendable* component);

  void EnableTelemetry(Sendable* component);
  void DisableTelemetry(Sendable* component);
  void DisableAllTelemetry();

  bool IsEnabled() const;
  void SetEnabled(bool enabled);

  void UpdateValues();

 private:
  LiveWindow();

  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace frc

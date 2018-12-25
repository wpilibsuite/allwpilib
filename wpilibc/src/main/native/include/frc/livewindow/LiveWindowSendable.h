/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <wpi/deprecated.h>

#include "frc/smartdashboard/Sendable.h"

namespace frc {

/**
 * Live Window Sendable is a special type of object sendable to the live window.
 * @deprecated Use Sendable directly instead
 */
class LiveWindowSendable : public Sendable {
 public:
  WPI_DEPRECATED("use Sendable directly instead")
  LiveWindowSendable() = default;
  LiveWindowSendable(LiveWindowSendable&&) = default;
  LiveWindowSendable& operator=(LiveWindowSendable&&) = default;

  /**
   * Update the table for this sendable object with the latest values.
   */
  virtual void UpdateTable() = 0;

  /**
   * Start having this sendable object automatically respond to value changes
   * reflect the value on the table.
   */
  virtual void StartLiveWindowMode() = 0;

  /**
   * Stop having this sendable object automatically respond to value changes.
   */
  virtual void StopLiveWindowMode() = 0;

  std::string GetName() const override;
  void SetName(const wpi::Twine& name) override;
  std::string GetSubsystem() const override;
  void SetSubsystem(const wpi::Twine& subsystem) override;
  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc

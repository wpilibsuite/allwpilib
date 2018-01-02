/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <support/deprecated.h>

#include "SmartDashboard/Sendable.h"

namespace frc {

/**
 * Live Window Sendable is a special type of object sendable to the live window.
 * @deprecated Use Sendable directly instead
 */
class WPI_DEPRECATED("use Sendable directly instead") LiveWindowSendable
    : public Sendable {
 public:
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
  void SetName(const llvm::Twine& name) override;
  std::string GetSubsystem() const override;
  void SetSubsystem(const llvm::Twine& subsystem) override;
  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
